#pragma once
#include <unordered_set>
#include <string>
#include<optional>
#include <string_view>


class HttpResource {
public:
	// Even though this is 
	enum class Scheme {
		HTTP,
		// HTTPS,
		UNKNOWN
	};

	static constexpr std::string_view schemeToString(const Scheme& scheme);
	static constexpr Scheme stringToScheme(const std::string_view schemeString);


	Scheme getScheme() const;
	bool isValid() const;

	std::string_view getAuthority() const;
	std::string_view getPath() const;

private:
	Scheme mScheme{ Scheme::UNKNOWN };
	std::string mAuthority;
	std::string mPath;
};

