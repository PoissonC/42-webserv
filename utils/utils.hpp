#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include <string>

typedef std::map<int, std::string> StatusCodeMap;

const StatusCodeMap &getStatusCodeMap();
std::string getStatusMessageFromCode(int code);

#endif // UTILS_HPP