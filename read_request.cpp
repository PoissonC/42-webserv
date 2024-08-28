/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:29 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/28 20:55:39 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Server;

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "handle_error_response.hpp"

static void finishReadingHeaders(std::vector<State>::iterator &state, Server & server) {
  std::cout << "Finished reading headers." << std::endl;
  state->req = Request(state->request_buff);
  std::map<std::string, std::string> headers = state->req.getHeaders();
  std::map<std::string, std::string>::iterator CL = headers.find("Content-Length");
  if (CL == headers.end()) {
    if (state->req.getMethod() == POST || state->req.getMethod() == DELETE) {
      handle_error_response(*state, 400, "Bad request.\nThe POST/DELETE request doesn't contain Content-Length in header", server);
      return;
    } else if (state->req.getMethod() == GET)
        state->contentLength = 0;
  } else {
    state->contentLength = std::strtol(CL->second.c_str(), NULL, 10);
    if (errno == ERANGE) {
      handle_error_response(*state, 400, "Bad request.\nThe Content-Length is invalid.", server);
      return;
    }
  }
}

void read_request(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;

  char buf[BUFFER_SIZE];
  bzero(buf, BUFFER_SIZE);
  ssize_t rc = recv(state->conn_fd, buf, BUFFER_SIZE - 1, MSG_DONTWAIT);

  // < 0 ..> an error occurs, = 0 client closes the connection
  if (rc <= 0) {
    std::cout << "close connection from read_request." << std::endl;
    server.close_conn(state->conn_fd, state);
    return;
  }

  state->request_buff += buf;

  state->bodyPos = state->request_buff.find("\r\n\r\n");
  if (state->bodyPos != std::string::npos && !state->isFinishHeaders) {
    finishReadingHeaders(state, server);
    state->isFinishHeaders = true;
  } else if (state->isFinishHeaders) {
    state->req = Request(state->request_buff);
  }

  if (rc < BUFFER_SIZE - 1 || state->contentLength == state->req.getBody().size()) {
    int status_code = state->req.checkRequest();
    if (status_code != 200) {
      handle_error_response(*state, status_code, "Bad request.\nThe request format is not valid.", server);
      return;
    }
	  handle_request(*state, server);
  }
}
