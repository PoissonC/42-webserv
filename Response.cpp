#include "Response.hpp"

// @def constructor
Response::Response()
    : _statusCode(200), _statusMessage("OK"), _body("Hello, World!") {
  _headers["Content-Type"] = "text/html";

//  _statusMessages[100] = "Continue";
//  _statusMessages[101] = "Switching Protocols";
  _statusMessages[200] = "OK";
//  _statusMessages[201] = "Created";
//  _statusMessages[202] = "Accepted";
//  _statusMessages[204] = "No Content";
//  _statusMessages[205] = "Reset Content";
//  _statusMessages[206] = "Partial Content";
//  _statusMessages[300] = "Multiple Choices";
  _statusMessages[301] = "Moved Permanently";
  _statusMessages[302] = "Found";
  _statusMessages[303] = "See Other";
//  _statusMessages[304] = "Not Modified";
  _statusMessages[307] = "Temporary Redirect";
  _statusMessages[308] = "Permanent Redirect";
  _statusMessages[400] = "Bad Request";
//  _statusMessages[401] = "Unauthorized";
  _statusMessages[403] = "Forbidden";
  _statusMessages[404] = "Not Found";
  _statusMessages[405] = "Method Not Allowed";
//  _statusMessages[406] = "Not Acceptable";
//  _statusMessages[408] = "Request Timeout";
//  _statusMessages[409] = "Conflict";
//  _statusMessages[410] = "Gone";
//  _statusMessages[411] = "Length Required";
//  _statusMessages[413] = "Payload Too Large";
//  _statusMessages[414] = "URI Too Long";
//  _statusMessages[415] = "Unsupported Media Type";
//  _statusMessages[416] = "Range Not Satisfiable";
//  _statusMessages[417] = "Expectation Failed";
//  _statusMessages[426] = "Upgrade Required";
  _statusMessages[500] = "Internal Server Error";
//  _statusMessages[501] = "Not Implemented";
//  _statusMessages[502] = "Bad Gateway";
//  _statusMessages[503] = "Service Unavailable";
//  _statusMessages[504] = "Gateway Timeout";
  _statusMessages[505] = "HTTP Version Not Supported";

 //link for status code: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status#information_responses

};

// @def destructor
Response::~Response(){};

// @defgroup setters
void Response::setStatusCode(int code) { _statusCode = code; }

void Response::setStatusMessage(const int code) {
  std::map<int, std::string>::const_iterator it = _statusMessages.find(code);
  if (it != _statusMessages.end()) {
	_statusMessage = it->second;
  }
  else {
	setStatusCode(500);
	_statusMessage = "Internal Server Error";
  }
}

void Response::setHeader(const std::string &key, const std::string &value) {
  _headers[key] = value;
}

void Response::setBody(const std::string &responseBody) {
  _body = responseBody;
}

// @defgroup getters
int Response::getStatusCode() const { return _statusCode; }

std::string Response::getStatusMessage() const { return _statusMessage; }

std::string Response::getHeader(const std::string &key) const {
  std::map<std::string, std::string>::const_iterator it = _headers.find(key);
  if (it != _headers.end()) {
    return it->second;
  }
  return "";
}

std::string Response::getBody() const { return _body; }

// @defgroup member functions
std::string Response::generateResponseString() const {
  std::ostringstream responseStream;

  responseStream << "HTTP/1.1 " << _statusCode << " " << _statusMessage
                 << "\r\n";

  if (_headers.find("Content-Length") == _headers.end()) {
    responseStream << "Content-Length: " << _body.size() << "\r\n";
  }

  for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
       it != _headers.end(); ++it) {
    responseStream << it->first << ": " << it->second << "\r\n";
  }

  responseStream << "\r\n" << _body;

  return responseStream.str();
}
