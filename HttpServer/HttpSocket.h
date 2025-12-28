#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string_view>
#include <string>

#pragma comment(lib,"Ws2_32.lib")



class HttpSocket {
public:
	HttpSocket(SOCKET listenSocket);
	~HttpSocket();
	
	// We will support move semantics but not value semantics
	HttpSocket(HttpSocket& other) = delete;
	HttpSocket& operator=(HttpSocket& other) = delete;

	HttpSocket(HttpSocket&& other);
	HttpSocket& operator=(HttpSocket&& other);

	void getRequest();
	void sendResponse(std::string_view response);

private:
	SOCKET mClientSocket{ INVALID_SOCKET };
};