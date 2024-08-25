#include "Request.hpp"

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
  this->_envCGI = NULL;
  parse();
}

Request::~Request() {
  // destructor
  if (this->_envCGI)
  {
	for (size_t i = 0; this->_envCGI[i] != NULL; i++) {
	  free(this->_envCGI[i]);
	}
	delete[] this->_envCGI;
  }
}

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
      bodyStream << line << std::endl;
    }
  }
  this->_body = bodyStream.str();
}

void Request::_parseLineReq(std::string line, std::string &method,
                            std::string &uri, std::string &version) {
  size_t pos = line.find(" ");
  if (pos != std::string::npos) {
    method = line.substr(0, pos);
    line.erase(0, pos + 1);
    pos = line.find(" ");
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

void Request::_parseUri(std::string uri, uriComponents &uriComponents) {
  std::string resource;
  std::string query;
  size_t pos = uri.find("?");
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
  pos = uriParse.find(":");
  if (pos != std::string::npos) {
    uriComponents.host = uriParse.substr(0, pos);
    uriParse.erase(0, pos + 1);
  }
  pos = uriParse.find("/");
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
  pos = uriParse.find("#");
  if (pos != std::string::npos) {
    uriComponents.query = uriParse.substr(0, pos);
    uriParse.erase(0, pos + 1);
    uriComponents.fragment = uriParse;
  } else {
    uriComponents.query = uriParse;
  }
}

int Request::checkRequest() {
  if (this->_method == "" || this->_uri == "" || this->_version == "")
    return (400);
  if (this->_method != "GET" && this->_method != "POST" &&
      this->_method != "DELETE")
    return (400);
  if (this->_version != "HTTP/1.1")
    return (400);
  if (this->_headers.find("Host") == this->_headers.end())
    return (400);
  if (this->_headers.find("Content-Length") != this->_headers.end()) {
    if (this->_body.empty())
      return (400);
  }
  std::string hostUri = this->_uriComponents.host;
  std::string hostHeader = this->_headers["Host"];
  if (!hostUri.empty()) {
    if (hostUri.find("www.") == 0)
      hostUri = hostUri.substr(4);
    if (hostHeader.find("www.") == 0)
      hostHeader = hostHeader.substr(4);
    if (hostHeader.find(":") != std::string::npos)
      hostHeader = hostHeader.substr(0, hostHeader.find(":"));
    if (hostUri != hostHeader)
      return (400);
  }
  for (unsigned long i = 0; i < _uriComponents.path.size() - 1; i++) {
    if (_uriComponents.path[i] == '/' && _uriComponents.path[i + 1] == '/')
      return (400);
  }
  return (200);
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

void Request::setEnvCGI(const std::string cgiPath, char ** env) {

  std::vector<std::string> envCGI;
  while (*env) {
	envCGI.push_back(*env);
	env++;
  }

	if (!_method.empty())
		envCGI.push_back("REQUEST_METHOD=" + _method);
	if (!_uri.empty())
		envCGI.push_back("QUERY_STRING=" + _uriComponents.query);
	if (_headers.find("Content-Type") != _headers.end())
		envCGI.push_back("CONTENT_TYPE=" + _headers.find("Content-Type")->second);
	if (_headers.find("Content-Length") != _headers.end())
		envCGI.push_back("CONTENT_LENGTH=" + _headers.find("Content-Length")->second);
	envCGI.push_back("PATH_INFO=" + cgiPath);

  char **envCGIChar = new char *[envCGI.size() + 1];
  for (size_t i = 0; i < envCGI.size(); i++) {
	envCGIChar[i] = strdup(envCGI[i].c_str());
  }
  envCGIChar[envCGI.size()] = NULL;
  this->_envCGI = envCGIChar;
}

char ** Request::getEnvCGI() {
  return (this->_envCGI);
}
