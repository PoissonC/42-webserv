#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include "LocationConfig.hpp"

struct uriComponents
{
	std::string scheme;
	std::string host;
	std::string port;
	std::string path;
	std::string query;
	std::string fragment;
};

class httpRequestParser
{
	public:
		httpRequestParser(const std::string &request);
		~httpRequestParser();
		void parse();
		int checkRequest();
		e_methods getMethod() const;
		std::string getUri() const;
		std::string getVersion() const;
		std::map<std::string, std::string> getHeaders() const;
		std::string getBody() const;
		uriComponents getUriComponents() const;

	private:
		std::string _req;
		std::string _method;
		std::string _uri;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		uriComponents _uriComponents;

		static void parseLineReq(std::string line, std::string &method, std::string &uri, std::string &version);
		static void parseHeaders(std::string line, std::map<std::string, std::string> &headers);
		static void parseUri(std::string uri, uriComponents &uriComponents);
};


#endif // HTTPREQUESTPARSER_HPP
