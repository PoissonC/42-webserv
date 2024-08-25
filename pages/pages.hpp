#ifndef PAGES_HPP
#define PAGES_HPP
#include "../State.hpp"
#include "../utils/HtmlMarkup.hpp"
#include <string>

std::string getIndexPage(State &state);
std::string getDirectoryPage(State &state);

#endif // PAGES_HPP