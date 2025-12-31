// HttpServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include "HttpMessage.h"
#include "HttpListener.h"
#include "HttpSocket.h"
#include <vector>
#include <iterator>
#include <optional>
#include <thread>
#include <mutex>
#include<queue>
#include <fstream>

#pragma comment(lib,"Ws2_32.lib")

#define COMM_PORT "8080"
#define DEFAULT_BUFLEN 512
#define CWD "" // We want to be flexible in where we allow the server admin to store their files, so this will likely be exposed to the user

void findServerDirectories() {
	std::ifstream reader{ "server_configs.txt" };
}

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

	// Initialize concurrent data structures and their respective mutexes
	std::mutex requestsMutex;
	std::queue<HttpMessage> requests;

	std::mutex clientsMutex;
	std::vector<HttpSocket> clients;
	clients.reserve(10000); // Preallocate space for clients to avoid reallocation during operation
	std::cout << "Listening on port " << COMM_PORT << "...\n";

	// Global state for filesystem access

	// TODO: Examine whether the try-catch block would be better fitted inside some of the other threads as opposed to main
	try {
		// Accept new client connections
		std::thread acceptThread([&clients, &result, &clientsMutex]() {
			HttpListener listener{ result };
			while (true) {
				std::optional<HttpSocket> potentialClient{ listener.acceptConnection() };
				if (potentialClient.has_value()) {
					const std::lock_guard<std::mutex> clientsLock(clientsMutex);
					clients.push_back(std::move(potentialClient.value()));
					//std::cout << "Accepted new client connection. Total clients: " << clients.size() << "\n";
				}
			}
		});

		std::thread processRequestThread([&requests,&requestsMutex]() {
			std::queue<HttpMessage> requests;
			while (true) {
				{
					const std::lock_guard requestsGuard{ requestsMutex };
				}
			}
			
			});

		acceptThread.detach();
		processRequestThread.detach();

		do {
			// Iterate through clients and service all requests
			const std::lock_guard<std::mutex> clientsLock(clientsMutex);
			for (auto it{ clients.begin() }; it != clients.end(); /* To avoid issues with iterator invalidation, do nothing here*/) {
				std::optional<std::string> request{ it->getRequest() };
				// Check if the client has disconnected
				if(it->shouldClose()) {
					// TODO: Assign unique client IDs to each client; don't just base their id on their place in the clients vector
					//std::cout << "Client #" << (std::distance(clients.begin(), it) + 1) << " has disconnected.\n";
					it = clients.erase(it);
					continue;
				}
				

				// Handle the client's request
				if (request.has_value()) {
					// Construct an HttpRequest object from the raw request string
					HttpMessage httpRequest{ request.value() };
					std::cout << "Received request for " << httpRequest.getRequestTarget() << "\n";
					std::cout << request.value() << "\n\n";
					std::string message;
					std::string responseBody;
					std::string line;
					std::ifstream resourceReader{ httpRequest.getRequestTarget().substr(1)};
					std::cout << resourceReader.is_open();
					switch (httpRequest.getMethod()) {
						case GET:
							message = "HTTP/1.1 200 OK\r\n";
							while (std::getline(resourceReader,line)) {
								responseBody += line;
							}

							message += "Content-Length: " + std::to_string(responseBody.length());
							message += "\r\n\r\n" + responseBody;

							std::cout << message;
							//std::string target_URI{ httpRequest.getRequestTarget() };
							//message = "HTTP/1.1 200 OK\nHost: localhost:8080" ;
							/*message = "HTTP/1.1 200 OK\r\n"
								"Server: localhost\r\n"
								"Content-Length:11\r\n"
								"\r\n"
								"twoaklcdefg";	*/
								/*"<!DOCTYPE HTML>"
								"<html>"
								"<body><1>Hello, message</h1></body>"
								"</html>"*/;
							
							//std::cout << message << "\n";
							it->sendResponse(message);
							break;
						default:
							throw "Not implemented yet. Goodbye world";
					}
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
