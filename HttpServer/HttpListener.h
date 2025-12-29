#pragma once
#include <optional>
#include "HttpSocket.h"

class HttpListener {
public:
	HttpListener() = delete;
	HttpListener(struct addrinfo* addrinfo);
	~HttpListener();
	
	// We will never support copy semantics for this class. For now, move semantics are also disabled
	HttpListener(HttpListener& other) = delete;
	HttpListener& operator=(HttpListener& other) = delete;
	HttpListener(HttpListener&& other) = delete;
	HttpListener& operator=(HttpListener&& other) = delete;

	std::optional<HttpSocket> acceptConnection();

private:
	u_long mMode = 1; // For non-blocking I/O
	SOCKET mListenSocket{ INVALID_SOCKET };
};

