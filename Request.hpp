#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "LocationConfig.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class State;

enum e_cookie {
  NO_COOKIE = 0,
  COOKIE_1 = 1,
  COOKIE_2 = 2,
  COOKIE_3 = 3,
  COOKIE_4 = 4,
  COOKIE_5 = 5,
  COOKIE_6 = 6,
  COOKIE_7 = 7,
  COOKIE_8 = 8,
  WRONG_COOKIE = 9
};

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
  int getCookie() const;

  void setEnvCGI(State &state, char **env);
  std::vector<char *> getEnvCGI() const;
  std::string createCookie() const;

private:
  std::string _req;
  std::string _method;
  std::string _uri;
  std::string _version;
  std::map<std::string, std::string> _headers;
  std::string _body;
  int _cookie;
  uriComponents _uriComponents;

  std::vector<std::string> _envCGI;

  void _findCookie(std::map<std::string, std::string> headers);
  static int _verifyCookie(const std::string &cookie);

  static void _parseLineReq(std::string line, std::string &method,
                            std::string &uri, std::string &version);
  static void _parseHeaders(std::string line,
                            std::map<std::string, std::string> &headers);
  static void _parseUri(const std::string &uri, uriComponents &uriComponents);
};

#endif // REQUEST_HPP
