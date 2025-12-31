#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <sstream>

// This unscoped enumeration is used to represent HTTP methods and is accessible to other parts of the program
enum HttpMethod {
	GET,
	POST,
	PUT,
	DEL, // Cannot use DELETE as it is a reserved word in the Windows SDK TODO: Is this actually from the windows sdk or vanilla c++?
	HEAD,
	OPTIONS,
	PATCH,
	UNKNOWN
};

class HttpMessage {
public:
	HttpMessage() = default; // Default initialization of all member variables is a valid state
	explicit HttpMessage(const std::string& rawRequest);
	void parseRequest(const std::string& rawReqest);

	// Class getters
	HttpMethod getMethod() const { return this->mMethod; }
	std::string getRequestTarget() const { return this->mRequestTarget; }
	std::string getHTTPVersion() const { return this->mHTTPVersion; }
	std::optional<std::string> getHeaderValue(const std::string& headerName) const;

	// Static utility functions for converting between string and HttpMethod enum
	static HttpMethod stringToHttpMethod(const std::string& methodStr);
	static std::string httpMethodToString(const HttpMethod& method);

private:
	HttpMethod mMethod{};
	std::string mRequestTarget;
	std::string mHTTPVersion;
	std::unordered_map<std::string, std::string> mHeaders;
	std::string mBody;
};