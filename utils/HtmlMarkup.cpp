#include "HtmlMarkup.hpp"

const std::string HtmlMarkup::_defaultTitle = "Webserver";
const std::string HtmlMarkup::_defaultHeaderContent = "<h1>Webserver</h1>";
const std::string HtmlMarkup::_defaultFooterContent = "<p>&copy 2024</p>";

const std::string HtmlMarkup::_defaultHtmlStyles =
    "html { scroll-behavior: smooth; margin: 0; padding: 0; }"
    "h1, h2, h3, h4, h5, h6, p { margin: 0; padding: 0; }"
    "a { text-decoration: none; color: #eee; } a:visited { color: #aaa }";
const std::string HtmlMarkup::_defaultHeaderStyles = "h1 { margin: 20px; }";
const std::string HtmlMarkup::_defaultBodyStyles =
    "body { "
    "font-family: Courier, slab-serif, sans-serif; color: #fff; "
    "background-color: #111; max-width: 1024px; margin: 0 auto; display: grid; "
    "grid-template-columns: minmax(1024px, 1fr);"
    "grid-template-rows: max-content auto max-content;"
    "min-height: 100vh; width: 100vw; }";
const std::string HtmlMarkup::_defaultMainStyles =
    "main { background-color: "
    "#eee; color: #111; padding: 20px; }";
const std::string HtmlMarkup::_defaultFooterStyles =
    "footer { background-color: #eee; color: #111; padding: 0 20px; }"
    "footer p { border-top: 0.1px solid #111; padding: 20px 0; opacity: 0.5 }";

// @def constructor
HtmlMarkup::HtmlMarkup() {
  _doctype = "<!DOCTYPE html>";

  // Add default title
  addTitle(_defaultTitle);

  // Add default styles
  addStyle(_defaultHtmlStyles);
  addStyle(_defaultBodyStyles);
  addStyle(_defaultMainStyles);
}

// @def destructor
HtmlMarkup::~HtmlMarkup() {}

// @defgroup member functions
void HtmlMarkup::addTitle(const std::string &title) {
  _headElements.push_back("<title>" + title + "</title>");
}

void HtmlMarkup::addStyle(const std::string &styleContent) {
  _styleElements.push_back("<style>" + styleContent + "</style>");
}

void HtmlMarkup::addBodyContent(const std::string &content) {
  _body += content;
}

void HtmlMarkup::addScript(const std::string &scriptContent) {
  _scriptElements.push_back(scriptContent);
}

void HtmlMarkup::addHeaderContent(const std::string &content) {
  _headerElements.push_back(content);
}

void HtmlMarkup::addMainContent(const std::string &content) {
  _mainElements.push_back(content);
}

void HtmlMarkup::addFooterContent(const std::string &content) {
  _footerElements.push_back(content);
}

std::string HtmlMarkup::generateMarkup() {
  // Assemble header element
  if (_headerElements.empty()) {
    addHeaderContent(_defaultHeaderContent);
    addStyle(_defaultHeaderStyles);
  }

  std::ostringstream headerContent;
  for (std::vector<std::string>::iterator it = _headerElements.begin();
       it != _headerElements.end(); ++it) {
    headerContent << *it;
  }
  _header = "<header>" + headerContent.str() + "</header>";

  // Assemble main element
  std::ostringstream mainContent;
  for (std::vector<std::string>::iterator it = _mainElements.begin();
       it != _mainElements.end(); ++it) {
    mainContent << *it;
  }
  _main = "<main>" + mainContent.str() + "</main>";

  // Assemble footer element
  if (_footerElements.empty()) {
    addFooterContent(_defaultFooterContent);
    addStyle(_defaultFooterStyles);
  }

  std::ostringstream footerContent;
  for (std::vector<std::string>::iterator it = _footerElements.begin();
       it != _footerElements.end(); ++it) {
    footerContent << *it;
  }
  _footer = "<footer>" + footerContent.str() + "</footer>";

  // Assemble script
  std::ostringstream scriptContent;
  for (std::vector<std::string>::iterator it = _scriptElements.begin();
       it != _scriptElements.end(); ++it) {
    scriptContent << *it;
  }
  _script = "<script>" + scriptContent.str() + "</script>";

  // Assemble body element
  _body = "<body>" + _header + _main + _footer + _script + "</body>";

  // Assemble head element
  for (std::vector<std::string>::iterator it = _styleElements.begin();
       it != _styleElements.end(); ++it) {
    _headElements.push_back(*it);
  }
  std::ostringstream headContent;
  for (std::vector<std::string>::iterator it = _headElements.begin();
       it != _headElements.end(); ++it) {
    headContent << *it;
    _head += *it;
  }
  _head = "<head>" + headContent.str() + "</head>";

  return _doctype + "<html>" + _head + _body + "</html>";
}