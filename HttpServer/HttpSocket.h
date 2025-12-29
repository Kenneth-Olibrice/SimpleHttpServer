#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string_view>
#include <string>
#include <array>
#include <optional>

#pragma comment(lib,"Ws2_32.lib")

// May want to increase or decrease this later. The buffer size should be slightly larger than the average HTTP request
#define BUFFER_SIZE 512 
#define NUM_CLIENTS_MAX 10000

class HttpSocket {
public:
	HttpSocket() {};
	HttpSocket(SOCKET listenSocket);
	~HttpSocket();
	
	// We will support move semantics but not value semantics
	HttpSocket(HttpSocket& other) = delete;
	HttpSocket& operator=(HttpSocket& other) = delete;

	HttpSocket(HttpSocket&& other);
	HttpSocket& operator=(HttpSocket&& other);

	std::optional<std::string> getRequest();
	void sendResponse(std::string_view response);
	void captureSocket(SOCKET socket) { this->mClientSocket = socket; }
	bool shouldClose() const { return this->mShouldClose; }
private:
	bool mShouldClose{ false };
	SOCKET mClientSocket{ INVALID_SOCKET };
	std::array<char,BUFFER_SIZE> mBuffer;
};