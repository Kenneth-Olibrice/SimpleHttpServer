#include "HttpSocket.h"

HttpSocket::HttpSocket(SOCKET listenSocket) {
	this->mClientSocket = accept(listenSocket, NULL, NULL);

	if(this->mClientSocket == INVALID_SOCKET) 
		throw "Failed to accept connection";
}

HttpSocket::~HttpSocket() {
	if (this->mClientSocket != INVALID_SOCKET) {
		closesocket(this->mClientSocket);
		this->mClientSocket = INVALID_SOCKET;
	}
}