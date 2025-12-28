#include "HttpRequest.h"

HttpRequest::HttpRequest(const std::string& rawRequest) {
	this->parseRequest(rawRequest);
}

std::optional<std::string> HttpRequest::getHeaderValue(const std::string& headerName) const {
	auto it = mHeaders.find(headerName);
	if (it != mHeaders.end()) {
		return it->second;
	}
	return std::nullopt;
}

HttpMethod HttpRequest::stringToHttpMethod(const std::string& methodStr) {
	if (methodStr == "GET") return HttpMethod::GET;
	if (methodStr == "POST") return HttpMethod::POST;
	if (methodStr == "PUT") return HttpMethod::PUT;
	if (methodStr == "DELETE") return HttpMethod::DEL;
	if (methodStr == "HEAD") return HttpMethod::HEAD;
	if (methodStr == "OPTIONS") return HttpMethod::OPTIONS;
	if (methodStr == "PATCH") return HttpMethod::PATCH;
	return HttpMethod::UNKNOWN;
}

std::string HttpRequest::httpMethodToString(const HttpMethod& method) {
	switch (method) {
	case HttpMethod::GET:
		return "GET";
		break;
	case HttpMethod::POST:
		return "POST";
		break;
	case HttpMethod::PUT:
		return "PUT";
		break;
	case HttpMethod::DEL:
		return "DELETE";
		break;
	case HttpMethod::HEAD:
		return "HEAD";
		break;
	case HttpMethod::OPTIONS:
		return "OPTIONS";
		break;
	case HttpMethod::PATCH:
		return "PATCH";
		break;
	default:
		return "UNKNOWN";
		break;
	}
}

void HttpRequest::parseRequest(const std::string& raw) {
	std::stringstream stream(raw);

	// Read the start line information and store it in the appropriate member variables
	std::string requestMethodStr;
	stream >> requestMethodStr;
	this->mMethod = HttpRequest::stringToHttpMethod(requestMethodStr);

	stream >> this->mRequestTarget;
	stream >> this->mHTTPVersion;
	
	// Now read the request headers into the headers map
	std::string buffer;
	while (std::getline(stream,buffer)) {
		if (buffer.length() == 0)
			break; // End of headers

		std::string headerName = buffer.substr(0, buffer.find(":"));
		std::string headerValue = buffer.substr(buffer.find(":") + 2, buffer.length() - headerName.length() - 2 - 1);

		this->mHeaders[headerName] = headerValue;
	}

	// Finally, read the request body
	while (std::getline(stream, buffer)) {
		this->mBody += buffer + "\n";
	}
}

