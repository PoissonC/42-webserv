#ifndef PAGES_HPP
#define PAGES_HPP
#include "../State.hpp"
#include "../utils/HtmlMarkup.hpp"
#include <string>
#include <iostream>
#include <iomanip>

std::string getIndexPage(State &state);
std::string getDirectoryPage(State &state);

#endif // PAGES_HPP