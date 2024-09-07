#include "pages.hpp"

std::string getErrorPage(int statusCode, const std::string &errorMessage) {
  // @defgroup elements styles
  const std::string layoutStyles =
      ".layout { height: 100%; display: flex; flex-direction: column; "
      "justify-content: center; }";
  const std::string statusCodeStyles =
      ".status-code { font-size: 44px; font-weight: 700; text-align: "
      "center; }";
  const std::string errorMessageStyles =
      ".error-message { width: 50%; font-size: 24px; text-align: center; "
      "margin: 0 auto; }";

  HtmlMarkup errorPage;
  std::ostringstream mainContent;

  mainContent << "<section class=\"layout\">";
  mainContent << "<p class=\"status-code\">" << statusCode << "</p>";
  mainContent << "<p class=\"error-message\">" << errorMessage << "</p>";
  mainContent << "</section>";

  errorPage.addMainContent(mainContent.str());

  errorPage.addStyle(layoutStyles);
  errorPage.addStyle(statusCodeStyles);
  errorPage.addStyle(errorMessageStyles);

  return errorPage.generateMarkup();
}