#include "HttpListener.h"

HttpListener::HttpListener(struct addrinfo* addrinfo) {
	this->mListenSocket = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
	if(this->mListenSocket == INVALID_SOCKET)
		throw "Could not listen at the specified address.";

	int iResult = bind(this->mListenSocket, addrinfo->ai_addr, static_cast<int>(addrinfo->ai_addrlen));
	
	if (iResult == SOCKET_ERROR) {
		closesocket(this->mListenSocket);
		this->mListenSocket = INVALID_SOCKET;
		throw "Could not bind to the specified address.";
	}

	iResult = listen(this->mListenSocket, SOMAXCONN_HINT(NUM_CLIENTS_MAX));

	if(iResult == SOCKET_ERROR) {
		closesocket(this->mListenSocket);
		this->mListenSocket = INVALID_SOCKET;
		throw "Could not listen on the specified address.";
	}
}

HttpListener::HttpListener(HttpListener&& other) : mListenSocket(other.mListenSocket) {
	other.mListenSocket = INVALID_SOCKET;
}

HttpListener& HttpListener::operator=(HttpListener&& other) {
	if (this == &other)
		return *this;

	if (this->mListenSocket != INVALID_SOCKET)
		closesocket(this->mListenSocket);

	this->mListenSocket = other.mListenSocket;
	other.mListenSocket = INVALID_SOCKET;

	return *this;
}

HttpListener::~HttpListener() {
	if(this->mListenSocket != INVALID_SOCKET)
		closesocket(this->mListenSocket);
}

std::optional<HttpSocket> HttpListener::acceptConnection() {
	SOCKET clientSocket{ accept(this->mListenSocket, NULL, NULL) };

	if (clientSocket == INVALID_SOCKET) {
		return std::nullopt;
	}
	else {
		int result{ ioctlsocket(clientSocket, FIONBIO, &this->mMode) }; // Set the socket's I/O mode to non-blocking

		// If, for some reason, we fail to set the I/O mode of the socket, then free its resource and do not return it
		if (result != NO_ERROR) {
			closesocket(clientSocket);
			return std::nullopt;
		}

		return HttpSocket(clientSocket);
	}
}