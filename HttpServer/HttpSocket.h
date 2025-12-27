#pragma once

class HttpSocket {
public:
	HttpSocket();
	~HttpSocket();

	// We will support move semantics but not value semantics
	HttpSocket(HttpSocket& other) = delete;
	HttpSocket& operator=(HttpSocket& other) = delete;

	HttpSocket(HttpSocket&& other);
	HttpSocket& operator=(HttpSocket&& other);

private:
	int socket; // Change datatype
};