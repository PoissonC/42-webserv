#include "Response.hpp"

// @def constructor
Response::Response()
    : _statusCode(200), _statusMessage("OK"), _body("Hello, World!") {
  _headers["Content-Type"] = "text/html";
};

// @def destructor
Response::~Response(){};

// @defgroup setters
void Response::setStatusCode(int code) { _statusCode = code; }

void Response::setStatusMessage(const std::string &message) {
  _statusMessage = message;
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
