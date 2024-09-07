/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_error.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 21:19:18 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/27 16:49:15 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_error.hpp"
#include "constants.hpp"
#include "pages/pages.hpp"

std::string getMimeType(const std::string &fileName);

int getStatusCode() {
  switch (errno) {
  case EACCES: // Permission denied
  case EROFS:  // Read-only file system
    return FORBIDDEN;
  case ENOENT:  // No such file or directory
  case ENOTDIR: // Not a directory
    return NOT_FOUND;
  case EISDIR:       // Is a directory
  case ENAMETOOLONG: // File name too long
    return BAD_REQUEST;
  case ENOSPC: // No space left on device
    return INSUFFICIENT_STORAGE;
  case ENFILE: // File table overflow
  case EMFILE: // Too many open files
    return SERVICE_UNAVAILABLE;
  default:
    return INTERNAL_SERVER_ERROR;
  }
}

void handle_error(State &state, int statusCode, const std::string &context,
                  Server &server) {
  std::string errorMessage;
  int finalStatusCode = statusCode;

  if (statusCode == UNDEFINED) {
    // Determine the error based on errno if statusCode is undefined
    errorMessage =
        (!context.empty() ? context + ": " : std::string()) + strerror(errno);
    finalStatusCode = getStatusCode();
  } else {
    // If statusCode is specified, use context as full message
    errorMessage = context;
  }

  state.res.setStatusCode(finalStatusCode);
  state.res.setHeader("Content-Type", "text/html");

  // Check if the server config has an error page for the status code
  std::map<int, std::string> error_pages = state.server.getErrorPages();
  std::map<int, std::string>::iterator it = error_pages.find(finalStatusCode);

  if (it != error_pages.end()) {
    state.file_fd = open(it->second.c_str(), O_RDONLY);
    if (state.file_fd >= 0) {
      state.res.setHeader("Content-Type", getMimeType(it->second));
      return wait_to_read_file(state, server);
    }
  }

  // Fallback to generated error page if not defined in server config
  state.res.setBody(getErrorPage(finalStatusCode, errorMessage));
  if (state.response_buff.empty())
    state.response_buff = state.res.generateResponseString();

  std::cerr << errorMessage << std::endl;

  server.remove_from_poll(state.cgi_pipe_r[0]);
  server.remove_from_poll(state.cgi_pipe_w[1]);
  server.remove_from_poll(state.file_fd);

  state.bytes_sent = 0;

  wait_to_send_resonpse(state, server);
}
