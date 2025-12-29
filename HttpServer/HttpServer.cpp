// HttpServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
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

	std::mutex clientsMutex;

	try {
		std::vector<HttpSocket> clients;
		clients.reserve(10000); // Preallocate space for clients to avoid reallocation during operation
		std::cout << "Listening on port " << COMM_PORT << "...\n";
		

		// Accept new client connections
		std::thread acceptThread([&clients, &result, &clientsMutex]() {
			HttpListener listener(result);
			while (true) {
				std::optional<HttpSocket> potentialClient{ listener.acceptConnection() };
				if (potentialClient.has_value()) {
					const std::lock_guard<std::mutex> clientsLock(clientsMutex);
					clients.push_back(std::move(potentialClient.value()));
					//std::cout << "Accepted new client connection. Total clients: " << clients.size() << "\n";
				}
			}
		});

		acceptThread.detach();

		do {
			// Accept new client connections
			/*std::optional<HttpSocket> potentialClient{ listener.acceptConnection() };
			if (potentialClient.has_value()) {
				clients.push_back(std::move(potentialClient.value()));
				std::cout << "Accepted new client connection. Total clients: " << clients.size() << "\n";
			}*/
			
			// Iterate through clients and service all requests
			const std::lock_guard<std::mutex> clientsLock(clientsMutex);
			for (auto it{ clients.begin() }; it != clients.end(); /* To avoid issues with iterator invalidation, do nothing here*/) {
				std::optional<std::string> request{ it->getRequest() };

				// Check if the client has disconnected
				if(it->shouldClose()) {
					std::cout << "Client #" << std::distance(clients.begin(), it) << " has disconnected.\n";
					it = clients.erase(it);
					continue;
				}
				else {
					++it;
				}

				// Handle the client's request
				if (request.has_value()) {
					// Construct an HttpRequest object from the raw request string
					HttpRequest httpRequest{ request.value() };
					std::cout << "Received request for " << httpRequest.getRequestTarget() << "\n";
				}
			}

			//std::cout << "Test of nonblocking\n";
		} while (true);
	}
	catch (...) {
		std::cerr << "Fatal error. Last WSA error reported: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}
	
	


	/*SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(listenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	
	freeaddrinfo(result);

	if (iResult == SOCKET_ERROR) {
		std::cerr << "Failed to bind to port " << COMM_PORT << ". Exiting.\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, 1) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error %ld\n" << WSAGetLastError();
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET clientSocket{ INVALID_SOCKET };
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Failed to accept incoming socket connection.";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	closesocket(listenSocket);*/
	WSACleanup();
	return 0;
}
