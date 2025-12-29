#include "HttpSocket.h"

HttpSocket::HttpSocket(SOCKET listenSocket) : mClientSocket(listenSocket) {}

HttpSocket::HttpSocket(HttpSocket&& other) {
	this->mClientSocket = other.mClientSocket;
	other.mClientSocket = INVALID_SOCKET;
}

HttpSocket& HttpSocket::operator=(HttpSocket&& other) {
	if (this == &other) return *this;

	if (this->mClientSocket != INVALID_SOCKET)
		closesocket(this->mClientSocket);
	
	this->mClientSocket = other.mClientSocket;
	other.mClientSocket = INVALID_SOCKET;

	return *this;
}

HttpSocket::~HttpSocket() {
	if (this->mClientSocket != INVALID_SOCKET)
		closesocket(this->mClientSocket);
}

std::optional<std::string> HttpSocket::getRequest() {
	int result = recv(this->mClientSocket, this->mBuffer.data(), this->mBuffer.size(), NULL);

	// Because we are performing non-blocking I/O, we may get SOCKET_ERROR if no data is available, even if the connection is still valid
	// It is possible that an actual error occurrd, and in this case the caller can check WSAGetLastError() to determine the error
	if (result > 0) {
		return std::string(this->mBuffer.data(), result);
	}
	else {
		// Connection has been gracefully closed
		if(result == 0)
			this->mShouldClose = true;

		return std::nullopt;
	}
}