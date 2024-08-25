#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "LocationConfig.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class State;

struct uriComponents {
  std::string scheme;
  std::string host;
  std::string port;
  std::string path;
  std::string query;
  std::string fragment;
};

class Request {
public:
  Request(const std::string &request);
  ~Request();
  void parse();
  int checkRequest();
  e_methods getMethod() const;
  std::string getUri() const;
  std::string getVersion() const;
  std::map<std::string, std::string> getHeaders() const;
  std::string getBody() const;
  uriComponents getUriComponents() const;

  void setEnvCGI(State & state, const char ** env);
  char ** getEnvCGI() const;


private:
  std::string _req;
  std::string _method;
  std::string _uri;
  std::string _version;
  std::map<std::string, std::string> _headers;
  std::string _body;
  uriComponents _uriComponents;

  std::vector<std::string> _envCGI;

  static void _parseLineReq(std::string line, std::string &method,
                            std::string &uri, std::string &version);
  static void _parseHeaders(std::string line,
                            std::map<std::string, std::string> &headers);
  static void _parseUri(std::string uri, uriComponents &uriComponents);
};

#endif // REQUEST_HPP
