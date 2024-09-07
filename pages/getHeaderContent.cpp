#include "pages.hpp"

const int MESSAGE_COUNT = 9;

static const std::string messages[MESSAGE_COUNT] = {
    "Can I help you or you just browsing?",
    "Ok, you can look around a bit",
    "Wow, you really like it here, huh...",
    "There are other websites, you know...",
    "You're practically family now!",
    "The server is starting to recognize you...",
    "I know your IP address, you know...",
    "Legend says you'll never leave..."};

std::string getMessageForCounter(int counter) {
  if (counter < 1 || counter > 9)
    return std::string();

  return messages[counter - 1];
}

std::string getHeaderContent(HtmlMarkup &page, State &state) {
  // @defgroup elements styles
  const std::string layoutStyles =
      "header { display: flex; justify-content: "
      "space-between; align-items: center; margin: 20px; }";
  const std::string sessionMessageStyles =
      ".sessionMessage { font-style: italic; opacity: 0; "
      "animation: fadeIn 1s ease-in-out forwards; }"
      "@keyframes fadeIn { "
      "  0% { opacity: 0; } "
      "  100% { opacity: 1; } "
      "}";

  std::ostringstream headerContent;

  headerContent << "<h1>Webserver</h1>";
  headerContent << "<p class=\"sessionMessage\">";
  headerContent << getMessageForCounter(state.req.getCookie());
  headerContent << "</p>";

  page.addStyle(layoutStyles);
  page.addStyle(sessionMessageStyles);

  return headerContent.str();
}