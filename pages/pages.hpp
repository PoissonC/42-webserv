#ifndef PAGES_HPP
#define PAGES_HPP
#include "../State.hpp"
#include "../utils/HtmlMarkup.hpp"
#include <iomanip>
#include <iostream>
#include <string>

std::string getIndexPage(State &state);
std::string getDirectoryPage(State &state);
std::string getErrorPage(int statusCode, const std::string &errorMessage);
std::string getHeaderContent(HtmlMarkup &page, State &state);

#endif // PAGES_HPP