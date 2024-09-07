#include "pages.hpp"

std::string getIndexPage(State &state) {
  // @defgroup elements styles
  const std::string ulStyles =
      "ul { list-style: none; display: flex; gap: 20px;"
      "padding: 0; margin: 0; }";
  const std::string liStyles =
      "li { cursor: pointer; background-color: #111; padding: 10px; "
      "transition: opacity 0.2s ease-in-out; }"
      "li:hover { opacity: 0.8; transition: opacity 0.2s ease-in-out; }";

  HtmlMarkup indexPage;

  std::ostringstream mainContent;
  const std::map<std::string, LocationConfig> &locations =
      state.server.getLocations();
  std::map<std::string, LocationConfig>::const_iterator it = locations.begin();
  if (it != locations.end())
    ++it;

  mainContent << "<section><ul>";
  for (; it != locations.end(); ++it) {
    const std::string &locationPath = it->first;
    mainContent << "<li>";
    mainContent << "<a href=\"" << locationPath << "\">";
    mainContent << locationPath;
    mainContent << "</a>";
    mainContent << "</li>";
  }
  mainContent << "</ul></section>";

  indexPage.addHeaderContent(getHeaderContent(indexPage, state));
  indexPage.addMainContent(mainContent.str());

  indexPage.addStyle(ulStyles);
  indexPage.addStyle(liStyles);

  return indexPage.generateMarkup();
}