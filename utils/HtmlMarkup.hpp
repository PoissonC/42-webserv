#ifndef HTMLMARKUP_HPP
#define HTMLMARKUP_HPP
#include <sstream>
#include <string>
#include <vector>

class HtmlMarkup {
private:
  // @defgroup elements
  std::vector<std::string> _headElements;
  std::vector<std::string> _styleElements;
  std::vector<std::string> _headerElements;
  std::vector<std::string> _mainElements;
  std::vector<std::string> _footerElements;
  std::vector<std::string> _scriptElements;

  // @defgroup element assembled strings
  std::string _doctype;
  std::string _head;
  std::string _styleContent;
  std::string _body;
  std::string _header;
  std::string _main;
  std::string _footer;
  std::string _script;

  // @defgroup default content
  static const std::string _defaultTitle;
  static const std::string _defaultHeaderContent;
  static const std::string _defaultFooterContent;

  // @defgroup default styles
  static const std::string _defaultHtmlStyles;
  static const std::string _defaultBodyStyles;
  static const std::string _defaultHeaderStyles;
  static const std::string _defaultMainStyles;
  static const std::string _defaultFooterStyles;

public:
  HtmlMarkup();
  ~HtmlMarkup();

  // @defgroup member functions
  void addTitle(const std::string &title);
  void addStyle(const std::string &styleContent);
  void addBodyContent(const std::string &content);
  void addScript(const std::string &scriptContent);

  void addHeaderContent(const std::string &content);
  void addMainContent(const std::string &content);
  void addFooterContent(const std::string &content);

  std::string generateMarkup();
};

#endif // HTMLMARKUP_HPP
