#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string_view>
#include <string>
#include <array>
#include <optional>
#include "HttpMessage.h"

#pragma comment(lib,"Ws2_32.lib") // TODO: Cleanup

// May want to increase or decrease these later. The buffer size should be slightly larger than the average HTTP request
#define BUFFER_SIZE 512 
#define NUM_CLIENTS_MAX 10000

class HttpSocket {
public:
	HttpSocket() = default;
	explicit HttpSocket(SOCKET clientSocket) : mClientSocket(clientSocket) {};
	~HttpSocket();
	
	// We will support move semantics but not value semantics
	HttpSocket(HttpSocket& other) = delete;
	HttpSocket& operator=(HttpSocket& other) = delete;
	HttpSocket(HttpSocket&& other);
	HttpSocket& operator=(HttpSocket&& other);

	// Resource setter in case the object is default constructed
	void claimSocket(SOCKET clientSocket) { this->mClientSocket = clientSocket; };

	// Connection methods
	std::optional<std::string> getRequest();
	void sendResponse(const std::string& response);
	void captureSocket(SOCKET socket) { this->mClientSocket = socket; }
	bool shouldClose() const { return this->mShouldClose; }

private:
	bool mShouldClose{ false };
	SOCKET mClientSocket{ INVALID_SOCKET };
	std::array<char, BUFFER_SIZE> mBuffer{}; // TODO: May want to make thie static in the future since one buffer per client could get costly
};