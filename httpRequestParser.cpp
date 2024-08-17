#include "httpRequestParser.hpp"

httpRequestParser::httpRequestParser(const std::string &request) : _req(request)
{
	// constructor will initialize the request variable
	this->_method = "";
	this->_uri = "";
	this->_version = "";
	this->_headers = std::map<std::string, std::string>();
	this->_body = "";
	this->_uriComponents = uriComponents();
	this->_uriComponents.scheme = "";
	this->_uriComponents.host = "";
	this->_uriComponents.port = "";
	this->_uriComponents.path = "";
	this->_uriComponents.query = "";
	this->_uriComponents.fragment = "";
}

httpRequestParser::~httpRequestParser()
{
	// destructor
}

void httpRequestParser::parse()
{
	std::istringstream iss(this->_req);
	std::string line;
	bool inLine = true;
	bool inHeaders = false;
	std::stringstream bodyStream;
	while (std::getline(iss, line))
	{
		if (inLine)
		{
			if (line.empty())
				break;
			parseLineReq(line, this->_method, this->_uri, this->_version);
			parseUri(this->_uri, this->_uriComponents);
			inLine = false;
			inHeaders = true;
		}
		if (inHeaders)
		{
			if (line == "\r" || line == "\r\n" || line == "\n")
			{
				inHeaders = false;
				continue;
			}
			parseHeaders(line, this->_headers);
		}
		else
		{
			bodyStream << line << std::endl;
		}
	}
	this->_body = bodyStream.str();
}

void httpRequestParser::parseLineReq(std::string line, std::string &method, std::string &uri, std::string &version)
{
	size_t pos = line.find(" ");
	if (pos != std::string::npos)
	{
		method = line.substr(0, pos);
		line.erase(0, pos + 1);
		pos = line.find(" ");
		if (pos != std::string::npos)
		{
			uri = line.substr(0, pos);
			line.erase(0, pos + 1);
			version = line;
			version.erase(version.find_last_not_of("\r\n") + 1);
		}
	}
}

void httpRequestParser::parseHeaders(std::string line, std::map<std::string, std::string> &headers)
{
	size_t pos = line.find(": ");
	if (pos != std::string::npos)
	{
		std::string key = line.substr(0, pos);
		line.erase(0, pos + 2);
		line.erase(line.find_last_not_of("\r\n") + 1);
		headers[key] = line;
	}
}

void httpRequestParser::parseUri(std::string uri, uriComponents &uriComponents)
{
	std::string ressource;
	std::string query;
	size_t pos = uri.find("?");
	if (pos != std::string::npos)
	{
		ressource = uri.substr(0, pos);
		query = uri.substr(pos + 1);
	}
	else
	{
		ressource = uri;
		query = "";
	}
	std::string uriParse = ressource;
	pos = uriParse.find("://");
	if (pos != std::string::npos)
	{
		uriComponents.scheme = uriParse.substr(0, pos);
		uriParse.erase(0, pos + 3);
	}
	pos = uriParse.find(":");
	if (pos != std::string::npos)
	{
		uriComponents.host = uriParse.substr(0, pos);
		uriParse.erase(0, pos + 1);
	}
	pos = uriParse.find("/");
	if (pos != std::string::npos)
	{
		if (uriComponents.host.empty())
			uriComponents.host = uriParse.substr(0, pos);
		else
			uriComponents.port = uriParse.substr(0, pos);
		uriParse.erase(0, pos + 1);
	}
	else if (uriComponents.host.empty())
	{
		uriComponents.host = uriParse;
		uriParse = "";
	}
	if (uriParse.empty())
	{
		uriComponents.path = "/";
	}
	else
	{
		uriComponents.path = "/" + uriParse;
	}
	uriParse = query;
	pos = uriParse.find("#");
	if (pos != std::string::npos)
	{
		uriComponents.query = uriParse.substr(0, pos);
		uriParse.erase(0, pos + 1);
		uriComponents.fragment = uriParse;
	}
	else
	{
		uriComponents.query = uriParse;
	}
}

int httpRequestParser::checkRequest()
{
	if (this->_method == "" || this->_uri == "" || this->_version == "")
		return (400);
	if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE")
		return (400);
	if (this->_version != "HTTP/1.1")
		return (400);
	if (this->_headers.find("Host") == this->_headers.end())
		return (400);
	if (this->_headers.find("Content-Length") != this->_headers.end())
	{
		if (this->_body.empty())
			return (400);
	}
	std::string hostUri = this->_uriComponents.host;
	std::string hostHeader = this->_headers["Host"];
	if (!hostUri.empty())
	{
		if (hostUri.find("www.") == 0)
			hostUri = hostUri.substr(4);
		if (hostHeader.find("www.") == 0)
			hostHeader = hostHeader.substr(4);
		if (hostHeader.find(":") != std::string::npos)
			hostHeader = hostHeader.substr(0, hostHeader.find(":"));
		if (hostUri != hostHeader)
			return (400);
	}
	return (200);
}

e_methods httpRequestParser::getMethod() const
{
	if (this->_method == "GET")
		return (GET);
	if (this->_method == "POST")
		return (POST);
	else {
		std::cout << this->_method << std::endl;
		return (DELETE);
	}
}

std::string httpRequestParser::getUri() const
{
	return (this->_uri);
}

std::string httpRequestParser::getVersion() const
{
	return (this->_version);
}

std::map<std::string, std::string> httpRequestParser::getHeaders() const
{
	return (this->_headers);
}

std::string httpRequestParser::getBody() const
{
	return (this->_body);
}

uriComponents httpRequestParser::getUriComponents() const
{
	return (this->_uriComponents);
}
