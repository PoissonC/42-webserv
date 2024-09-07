/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/28 15:24:12 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
#include "helper.hpp"

static bool cgi_parser(State &state) {
  size_t headerEndPos = state.cgi_buff.find("\r\n\r\n");
  if (headerEndPos == std::string::npos) {
    state.res.setBody(state.cgi_buff);
    state.response_buff = state.res.generateResponseString();
    return true;
  }
  std::vector<std::string> headers =
      split(state.cgi_buff.substr(0, headerEndPos), ':');
  if (headers.size() % 2 != 0)
    return false;
  for (std::vector<std::string>::iterator it = headers.begin();
       it != headers.end(); it++) {
    std::vector<std::string>::iterator key = it++;
    state.res.setHeader(*key, (*it).substr(1));
  }
  state.res.setBody(state.cgi_buff.substr(headerEndPos + 4));
  state.response_buff = state.res.generateResponseString();
  return true;
}

void read_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd,
              Server &server) {
  if (!(pfd.revents & (POLLIN | POLLHUP)))
    return;
  char buf[BUFFER_SIZE];
  bzero(buf, sizeof(buf));

  ssize_t rc = read(state->cgi_pipe_r[0], buf, BUFFER_SIZE - 1);

  if (rc < 0) {
    close(state->cgi_pipe_r[0]);
    server.remove_from_poll(state->cgi_pipe_r[0]);
    return handle_error(*state, INTERNAL_SERVER_ERROR, READ_CGI_OUTPUT_FAILURE,
                        server);
  }
  state->cgi_buff += buf;

  // end of reading
  if (rc < BUFFER_SIZE - 1) {
    close(state->cgi_pipe_r[0]);
    server.remove_from_poll(state->cgi_pipe_r[0]);
    if (!cgi_parser(*state))
      return handle_error(*state, INTERNAL_SERVER_ERROR,
                          READ_CGI_OUTPUT_FAILURE, server);

    wait_to_send_resonpse(*state, server);
  }
}