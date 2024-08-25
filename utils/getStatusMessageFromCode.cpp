#include "utils.hpp"

const StatusCodeMap &getStatusCodeMap() {
  static StatusCodeMap statusCodeMap;
  if (statusCodeMap.empty()) {
    statusCodeMap[100] = "Continue";
    statusCodeMap[101] = "Switching Protocols";
    statusCodeMap[200] = "OK";
    statusCodeMap[201] = "Created";
    statusCodeMap[202] = "Accepted";
    statusCodeMap[203] = "Non-Authoritative Information";
    statusCodeMap[204] = "No Content";
    statusCodeMap[205] = "Reset Content";
    statusCodeMap[206] = "Partial Content";
    statusCodeMap[300] = "Multiple Choices";
    statusCodeMap[301] = "Moved Permanently";
    statusCodeMap[302] = "Found";
    statusCodeMap[303] = "See Other";
    statusCodeMap[304] = "Not Modified";
    statusCodeMap[305] = "Use Proxy";
    statusCodeMap[307] = "Temporary Redirect";
    statusCodeMap[400] = "Bad Request";
    statusCodeMap[401] = "Unauthorized";
    statusCodeMap[402] = "Payment Required";
    statusCodeMap[403] = "Forbidden";
    statusCodeMap[404] = "Not Found";
    statusCodeMap[405] = "Method Not Allowed";
    statusCodeMap[406] = "Not Acceptable";
    statusCodeMap[407] = "Proxy Authentication Required";
    statusCodeMap[408] = "Request Time-out";
    statusCodeMap[409] = "Conflict";
    statusCodeMap[410] = "Gone";
    statusCodeMap[411] = "Length Required";
    statusCodeMap[412] = "Precondition Failed";
    statusCodeMap[413] = "Request Entity Too Large";
    statusCodeMap[414] = "Request-URI Too Large";
    statusCodeMap[415] = "Unsupported Media Type";
    statusCodeMap[416] = "Requested range not satisfiable";
    statusCodeMap[417] = "Expectation Failed";
    statusCodeMap[500] = "Internal Server Error";
    statusCodeMap[501] = "Not Implemented";
    statusCodeMap[502] = "Bad Gateway";
    statusCodeMap[503] = "Service Unavailable";
    statusCodeMap[504] = "Gateway Time-out";
    statusCodeMap[505] = "HTTP Version not supported";
  }
  return statusCodeMap;
}

std::string getStatusMessageFromCode(int code) {
  const StatusCodeMap &statusCodeMap = getStatusCodeMap();
  StatusCodeMap::const_iterator it = statusCodeMap.find(code);
  if (it != statusCodeMap.end()) {
    return it->second;
  } else {
    return "Unknown Status";
  }
}