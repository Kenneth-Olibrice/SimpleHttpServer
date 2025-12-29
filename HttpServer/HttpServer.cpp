// HttpServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include "HttpRequest.h"
#include "HttpListener.h"
#include "HttpSocket.h"
#include <vector>
#include <iterator>
#include <optional>
#include <thread>
#include <mutex>

#pragma comment(lib,"Ws2_32.lib")

#define COMM_PORT "8080"
#define DEFAULT_BUFLEN 512

int main()
{
	// Initialize Winsock
	WSAData wsadata;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0) {
		std::cerr << "Failed to initialize Winsock\n";
		return 1;
	}

	// Create and configure the tcp socket we will be using for communication
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints; // What is this syntax?? TODO: find out
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, COMM_PORT, &hints, &result);
	if (iResult != 0) {
		std::cerr << "Could not find socket at port " << COMM_PORT << ". Exiting.\n";
		WSACleanup();
		return 1;
	}

	// Initialize the clients mutex here so it exists outside of the scope of the try-catch block
	std::mutex clientsMutex;

	try {
		std::vector<HttpSocket> clients;
		clients.reserve(10000); // Preallocate space for clients to avoid reallocation during operation
		std::cout << "Listening on port " << COMM_PORT << "...\n";
		

		// Accept new client connections
		std::thread acceptThread([&clients, &result, &clientsMutex]() {
			HttpListener listener{ result };
			while (true) {
				std::optional<HttpSocket> potentialClient{ listener.acceptConnection() };
				if (potentialClient.has_value()) {
					const std::lock_guard<std::mutex> clientsLock(clientsMutex);
					clients.push_back(std::move(potentialClient.value()));
					std::cout << "Accepted new client connection. Total clients: " << clients.size() << "\n";
				}
			}
		});

		acceptThread.detach();

		do {
			// Iterate through clients and service all requests
			const std::lock_guard<std::mutex> clientsLock(clientsMutex);
			for (auto it{ clients.begin() }; it != clients.end(); /* To avoid issues with iterator invalidation, do nothing here*/) {
				std::optional<std::string> request{ it->getRequest() };

				// Check if the client has disconnected
				if(it->shouldClose()) {
					// TODO: Assign unique client IDs to each client; don't just base their id on their place in the clients vector
					std::cout << "Client #" << (std::distance(clients.begin(), it) + 1) << " has disconnected.\n";
					it = clients.erase(it);
					continue;
				}
				

				// Handle the client's request
				if (request.has_value()) {
					// Construct an HttpRequest object from the raw request string
					HttpRequest httpRequest{ request.value() };
					std::cout << "Received request for " << httpRequest.getRequestTarget() << "\n";
				}

				++it;
			}
		} while (true);
	}
	catch (...) {
		std::cerr << "Fatal error. Last WSA error reported: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}

	// Final program cleanup. No need to free resource since all of our objects are RAII compliant
	WSACleanup();
	return 0;
}
