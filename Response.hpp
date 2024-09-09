#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "utils/utils.hpp"
#include <map>
#include <sstream>
#include <string>

class Response {
private:
  int _statusCode;
  std::string _statusMessage;
  std::map<std::string, std::string> _headers;
  std::string _body;

public:
  Response();
  ~Response();

  // @defgroup setters
  void setStatusCode(int code);
  void setHeader(const std::string &key, const std::string &value);
  void setBody(const std::string &responseBody);

  // @defgroup getters
  int getStatusCode() const;
  std::string getStatusMessage() const;
  std::string getHeader(const std::string &key) const;
  std::string getBody() const;

  // @defgroup member functions
  std::string generateResponseString() const;
};

#endif // RESPONSE_HPP
