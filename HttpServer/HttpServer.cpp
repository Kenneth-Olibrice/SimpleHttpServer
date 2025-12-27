// HttpServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib,"Ws2_32.lib")

#define COMM_PORT "8080"
#define DEFAULT_BUFLEN 512

typedef struct HTTPRequest {
	char startLine[128];
	char headers[1024];
	char body[1024];
} HTTPRequest;


int main()
{
	// Initialize Winsock
	WSAData wsadata;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iResult != 0) {
		std::cerr << "Failed to initialzie Winsock\n";
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

	SOCKET listenSocket = INVALID_SOCKET;
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

	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(clientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);


	closesocket(listenSocket);
	WSACleanup();
}
