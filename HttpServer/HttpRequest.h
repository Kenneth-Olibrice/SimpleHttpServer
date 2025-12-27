#pragma once
#include <string>
#include <string_view>
#include <optional>

typedef struct HttpRequest {
	std::string startLine;
	std::string headers;
	std::string body;
} HttpRequest;

// This unscoped enumeration is used to represent HTTP methods and is accessible to other parts of the program
enum HttpMethod {
	GET,
	POST,
	PUT,
	DELETE,
	HEAD,
	OPTIONS,
	PATCH,
	UNKNOWN
};

class HttpRequest {
public:
	HttpRequest() = default;
	HttpRequest(std::string_view raw);

	static std::optional<std::string> parseRequest(std::string_view request);

	std::string getStartLine() const;
	std::string getHeaders() const;
	std::string getHeader(std::string) const;
	std::string getBody() const;

private:
	HttpMethod mMethod;
};