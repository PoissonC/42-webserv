#include "Request.hpp"
#include "State.hpp"
#include "constants.hpp"

Request::Request(const std::string &request) : _req(request) {
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
  this->_cookie = NO_COOKIE;
  this->_envCGI = std::vector<std::string>();
  parse();
}

Request::~Request() {}

void Request::parse() {
  std::istringstream iss(this->_req);
  std::string line;
  bool inLine = true;
  bool inHeaders = false;
  std::stringstream bodyStream;
  while (std::getline(iss, line)) {
    if (inLine) {
      if (line.empty())
        break;
      _parseLineReq(line, this->_method, this->_uri, this->_version);
      _parseUri(this->_uri, this->_uriComponents);
      inLine = false;
      inHeaders = true;
    }
    if (inHeaders) {
      if (line == "\r" || line == "\r\n" || line == "\n") {
        inHeaders = false;
        continue;
      }
      _parseHeaders(line, this->_headers);
    } else {
      if (!iss.eof())
        bodyStream << line << std::endl;
      else
        bodyStream << line;
    }
  }
  this->_body = bodyStream.str();
  _findCookie(this->_headers);
  // TESTER COOKIE
  //  if (getHeaders().find("Cookie") == getHeaders().end())
  // 	std::cerr << "No cookie" << std::endl;
  //  else
  // 	std::cerr << getHeaders().find("Cookie")->second << std::endl;
  //  std::cerr << "Cookie: " << this->_cookie << std::endl;
  //  std::cerr << createCookie() << std::endl;
}

void Request::_parseLineReq(std::string line, std::string &method,
                            std::string &uri, std::string &version) {
  size_t pos = line.find(' ');
  if (pos != std::string::npos) {
    method = line.substr(0, pos);
    line.erase(0, pos + 1);
    pos = line.find(' ');
    if (pos != std::string::npos) {
      uri = line.substr(0, pos);
      line.erase(0, pos + 1);
      version = line;
      version.erase(version.find_last_not_of("\r\n") + 1);
    }
  }
}

void Request::_parseHeaders(std::string line,
                            std::map<std::string, std::string> &headers) {
  size_t pos = line.find(": ");
  if (pos != std::string::npos) {
    std::string key = line.substr(0, pos);
    line.erase(0, pos + 2);
    line.erase(line.find_last_not_of("\r\n") + 1);
    headers[key] = line;
  }
}

void Request::_parseUri(const std::string &uri, uriComponents &uriComponents) {
  std::string resource;
  std::string query;
  size_t pos = uri.find('?');
  if (pos != std::string::npos) {
    resource = uri.substr(0, pos);
    query = uri.substr(pos + 1);
  } else {
    resource = uri;
    query = "";
  }
  std::string uriParse = resource;
  pos = uriParse.find("://");
  if (pos != std::string::npos) {
    uriComponents.scheme = uriParse.substr(0, pos);
    uriParse.erase(0, pos + 3);
  }
  pos = uriParse.find(':');
  if (pos != std::string::npos) {
    uriComponents.host = uriParse.substr(0, pos);
    uriParse.erase(0, pos + 1);
  }
  pos = uriParse.find('/');
  if (pos != std::string::npos) {
    if (uriComponents.host.empty())
      uriComponents.host = uriParse.substr(0, pos);
    else
      uriComponents.port = uriParse.substr(0, pos);
    uriParse.erase(0, pos + 1);
  } else if (uriComponents.host.empty()) {
    uriComponents.host = uriParse;
    uriParse = "";
  }
  if (uriParse.empty()) {
    uriComponents.path = "/";
  } else {
    uriComponents.path = "/" + uriParse;
  }
  uriParse = query;
  pos = uriParse.find('#');
  if (pos != std::string::npos) {
    uriComponents.query = uriParse.substr(0, pos);
    uriParse.erase(0, pos + 1);
    uriComponents.fragment = uriParse;
  } else {
    uriComponents.query = uriParse;
  }
}

int Request::checkRequest() {
  if (this->_method.empty() || this->_uri.empty() || this->_version.empty())
    return (BAD_REQUEST);
  if (this->_method != "GET" && this->_method != "POST" &&
      this->_method != "DELETE")
    return (METHOD_NOT_ALLOWED);
  if (this->_version != "HTTP/1.1")
    return (HTTP_VERSION_NOT_SUPPORTED);
  if (this->_headers.find("Host") == this->_headers.end())
    return (BAD_REQUEST);
  std::map<std::string, std::string>::iterator CL =
      _headers.find("Content-Length");
  if (CL != this->_headers.end()) {
    std::strtol(CL->second.c_str(), NULL, 10);
    if (errno == ERANGE) {
      return (BAD_REQUEST);
    }
  } else if (_method == "POST")
    return (LENGTH_REQUIRED);
  std::string hostUri = this->_uriComponents.host;
  std::string hostHeader = this->_headers["Host"];
  if (!hostUri.empty()) {
    if (hostUri.find("www.") == 0)
      hostUri = hostUri.substr(4);
    if (hostHeader.find("www.") == 0)
      hostHeader = hostHeader.substr(4);
    if (hostHeader.find(':') != std::string::npos)
      hostHeader = hostHeader.substr(0, hostHeader.find(':'));
    if (hostUri != hostHeader)
      return (BAD_REQUEST);
  }
  for (unsigned long i = 0; i < _uriComponents.path.size() - 1; i++) {
    if (_uriComponents.path[i] == '/' && _uriComponents.path[i + 1] == '/')
      return (BAD_REQUEST);
  }
  return (OK);
}

e_methods Request::getMethod() const {
  if (this->_method == "GET")
    return (GET);
  if (this->_method == "POST")
    return (POST);
  if (this->_method == "DELETE")
    return (DELETE);
  else
    return (ERROR);
}

std::string Request::getUri() const { return (this->_uri); }

std::string Request::getVersion() const { return (this->_version); }

std::map<std::string, std::string> Request::getHeaders() const {
  return (this->_headers);
}

std::string Request::getBody() const { return (this->_body); }

uriComponents Request::getUriComponents() const {
  return (this->_uriComponents);
}

void Request::setEnvCGI(State &state, char **env) {
  while (*env) {
    _envCGI.push_back(*env);
    env++;
  }

  _envCGI.push_back("GATEWAY_INTERFACE=CGI/1.1");
  _envCGI.push_back("SERVER_PROTOCOL=HTTP/1.1");
  _envCGI.push_back("SERVER_SOFTWARE=MyC++Server/1.0");
  _envCGI.push_back("REDIRECT_STATUS=200");
  if (!_method.empty())
    _envCGI.push_back("REQUEST_METHOD=" + _method);
  if (!_uri.empty())
    _envCGI.push_back("REQUEST_URI=" + _uri);
  if (!_uriComponents.host.empty())
    _envCGI.push_back("SERVER_NAME=" + _uriComponents.host);
  if (!_uriComponents.port.empty())
    _envCGI.push_back("SERVER_PORT=" + _uriComponents.port);
  if (!_uri.empty())
    _envCGI.push_back("QUERY_STRING=" + _uriComponents.query);
  if (!_uriComponents.path.empty())
    _envCGI.push_back("SCRIPT_NAME=" + _uriComponents.path);
  if (!_headers.find("Host")->second.empty())
    _envCGI.push_back("HTTP_HOST=" + _headers.find("Host")->second);
  if (_headers.find("Content-Type") != _headers.end())
    _envCGI.push_back("CONTENT_TYPE=" + _headers.find("Content-Type")->second);
  if (_headers.find("Content-Length") != _headers.end() && _method != "GET")
    _envCGI.push_back("CONTENT_LENGTH=" +
                      _headers.find("Content-Length")->second);
  _envCGI.push_back("PATH_INFO=" + state.cgi_path);
  _envCGI.push_back("SCRIPT_FILENAME=" + state.cgi_path);
  _envCGI.push_back("DOCUMENT_ROOT=" + state.cgi_path);
  _envCGI.push_back("REMOTE_ADDR=" + state.client_ip_str);
}

std::vector<char *> Request::getEnvCGI() const {
  std::vector<char *> ret;
  for (std::vector<std::string>::const_iterator it = _envCGI.begin(); it != _envCGI.end(); it++) {
    ret.push_back(const_cast<char *>(it->c_str()));
  }
  ret.push_back(NULL);
  return (ret);
}

void Request::_findCookie(std::map<std::string, std::string> headers) {
  std::string cookie;
  if (headers.find("Cookie") != headers.end()) {
    cookie = headers.find("Cookie")->second;
  } else
    cookie = "";
  _cookie = _verifyCookie(cookie);
}

int Request::_verifyCookie(const std::string &cookie) {
  if (cookie.empty())
    return NO_COOKIE;

  size_t delimiterPos = cookie.find('=');
  if (delimiterPos == std::string::npos || delimiterPos == cookie.length() - 1)
    return WRONG_COOKIE;

  std::string valueStr = cookie.substr(delimiterPos + 1);

  char *end;
  long cookieValue = std::strtol(valueStr.c_str(), &end, 10);

  if (*end != '\0')
    return WRONG_COOKIE;

  // Check the value range
  if (cookieValue < NO_COOKIE || cookieValue >= WRONG_COOKIE)
    return WRONG_COOKIE;

  return static_cast<int>(cookieValue);
}

int Request::getCookie() const { return (this->_cookie); }

std::string Request::createCookie() const {
  std::stringstream cookie;

  cookie << "Cookie=";
  if (_cookie == NO_COOKIE)
    cookie << "1";
  else if (_cookie == COOKIE_8)
    cookie << COOKIE_8;
  else if (_cookie < WRONG_COOKIE)
    cookie << _cookie + 1;
  else {
    return "";
  }

  cookie << "; Path=/; Max-Age=" << COOKIE_MAX_AGE
         << "; Secure; HttpOnly; SameSite=Strict";

  return cookie.str();
}
