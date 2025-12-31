#include "HttpSocket.h"
#include <iostream> // Temporay, for testing purposes only

HttpSocket::HttpSocket(HttpSocket&& other) : mClientSocket(other.mClientSocket) {
	this->mClientSocket = other.mClientSocket;
	other.mClientSocket = INVALID_SOCKET;
}

HttpSocket& HttpSocket::operator=(HttpSocket&& other) {
	if (this == &other) return *this;

	if (this->mClientSocket != INVALID_SOCKET)
		closesocket(this->mClientSocket);
	
	this->mShouldClose = other.mShouldClose;
	this->mClientSocket = other.mClientSocket;
	other.mClientSocket = INVALID_SOCKET;

	return *this;
}

HttpSocket::~HttpSocket() {
	if (this->mClientSocket != INVALID_SOCKET)
		closesocket(this->mClientSocket);
}

std::optional<std::string> HttpSocket::getRequest() {
	int result = recv(this->mClientSocket, this->mBuffer.data(), this->mBuffer.size() - 1, NULL);
	std::string toReturn;

	// If the first read attempt results in WSAEWOULDBLOCK it means there's nothing to read
	if (result == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
		return std::nullopt;

	while (result > 0) {
		if (result == this->mBuffer.size() - 1) {
			this->mBuffer[this->mBuffer.size() - 1] = 0;
		}
		else {
			this->mBuffer[result] = 0;
		}

		toReturn += std::string(this->mBuffer.data());
		result = recv(this->mClientSocket, this->mBuffer.data(), this->mBuffer.size() - 1, NULL);
	}
	
	switch (result) {
	case 0:
		this->mShouldClose = true;
		return std::nullopt;
		break;
	case SOCKET_ERROR:
		// WSAEWOULDBLOCK occurs when the connection is healthy but we want to avoid blocking behavior
		// If any other errror occurs, then a genuine transport-layer problem has arisen in the program and the connection should be immediately terminated
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return toReturn;
		}
		else {
			this->mShouldClose = true;
			return std::nullopt;
		}
		break;
	default:
		// According to Winsock docs, this path is unreachable because recv returns 0 or SOCKET_ERROR
		// Technically this path is a candidate for std::unreachable, but we're not that bold
		std::cerr << "Fatal error ocurred while trying to read from HttpSocket" << std::endl;
		std::abort();
		break;
	}
}

void HttpSocket::sendResponse(const std::string& response) {
	int result = send(this->mClientSocket, response.data(), response.length(), NULL);
	if (result == SOCKET_ERROR) throw "Goodbye world.";
}