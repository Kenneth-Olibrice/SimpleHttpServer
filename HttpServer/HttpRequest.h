#pragma once
#include "HttpMessage.h"

class HttpRequest : public HttpMessage {
public:
	//HttpMessage() = default; // Default initialization of all member variables is a valid state
	//explicit HttpMessage(const std::string& rawRequest);
	void parseRequest(const std::string& rawReqest);

	// Class getters
	HttpMethod getMethod() const { return this->mMethod; }
	std::string getRequestTarget() const { return this->mRequestTarget; }
	

private:
	HttpMethod mMethod{};
	std::string mRequestTarget;
	std::string mBody;
};

