/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/09 21:57:17 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
#include "helper.hpp"

bool isValidStatusCode(std::string statusCodeString) {
  if (statusCodeString.length() < 3)
    return false;

  for (int i = 0; statusCodeString[i]; ++i) {
    if (!std::isdigit(statusCodeString[i]))
      return false;
  }

  char *end;
  long statusCode = std::strtol(statusCodeString.c_str(), &end, 10);

  if (end == statusCodeString.c_str() || statusCode > 599)
    return false;

  return true;
}

int populateResFromCgiOutput(State &state) {
  size_t headersEndPos = state.cgi_buff.find("\r\n\r\n");
  if (headersEndPos == std::string::npos)
    return FAILURE;

  std::string headers = state.cgi_buff.substr(0, headersEndPos);
  std::string body =
      state.cgi_buff.substr(headersEndPos + 4); // Skip "\r\n\r\n"

  std::istringstream headerStream(headers);
  std::string headerLine;
  bool isStatusFound = false;

  while (std::getline(headerStream, headerLine)) {
    if (headerLine.empty())
      continue;

    while (*headerLine.rbegin() == '\r')
      headerLine.resize(headerLine.size() - 1); // Remove trailing '\r'

    size_t delimiterPos = headerLine.find(": ");

    if (delimiterPos == std::string::npos) {
      return FAILURE;
    } else {
      std::string key = headerLine.substr(0, delimiterPos);
      std::string value = headerLine.substr(delimiterPos + 2);

      if (key == "Status") {
        if (isStatusFound)
          continue;

        isStatusFound = true;
        std::string statusCodeString = value.substr(0, 3);
        if (isValidStatusCode(statusCodeString)) {
          long statusCode = std::strtol(value.substr(0, 3).c_str(), NULL, 10);
          state.res.setStatusCode(static_cast<int>(statusCode));
        } else {
          return FAILURE;
        }
      } else {
        state.res.setHeader(key, value);
      }
    }
  }

  if (!isStatusFound)
    state.res.setStatusCode(OK);
  state.res.setBody(body);

  return SUCCESS;
}

void read_cgi(std::list<State>::iterator &state, const struct pollfd &pfd,
              Server &server) {
  if (!(pfd.revents & (POLLIN | POLLHUP)))
    return;
  char buf[BUFFER_SIZE];

  ssize_t rc = read(state->cgi_pipe_r[0], buf, BUFFER_SIZE);

  if (rc < 0) {
    close(state->cgi_pipe_r[0]);
    server.remove_from_poll(state->cgi_pipe_r[0]);
    state->cgi_pipe_r[0] = 0;
    return handle_error(*state, INTERNAL_SERVER_ERROR, READ_CGI_OUTPUT_FAILURE,
                        server);
  }
  state->cgi_buff.append(buf, rc);

  // end of reading
  if (rc < BUFFER_SIZE) {
    close(state->cgi_pipe_r[0]);
    server.remove_from_poll(state->cgi_pipe_r[0]);
    state->cgi_pipe_r[0] = 0;
    if (populateResFromCgiOutput(*state) == FAILURE) {
      return handle_error(*state, INTERNAL_SERVER_ERROR,
                          WRONG_FORMAT_CGI_OUTPUT, server);
    }

    state->response_buff = state->res.generateResponseString();
    wait_to_send_resonpse(*state, server);
  }
}