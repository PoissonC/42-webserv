/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/08 20:31:14 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
#include "handle_stages.hpp"

void write_cgi(std::list<State>::iterator &state, const struct pollfd &pfd,
               Server &server) {
  if (!(pfd.revents & POLLOUT))
    return;
  if (state->cgi_pipe_w[1] != pfd.fd) {
    std::cout << "Conn fd: " << state->conn_fd
              << ", pipew: " << state->cgi_pipe_w[1] << ", pfd: " << pfd.fd
              << std::endl;
    throw std::runtime_error("Control Flow is Wrong\n");
    return;
  }

  ssize_t wc = write(pfd.fd, state->cgi_buff.c_str() + state->bytes_sent,
                     state->cgi_buff.size() - state->bytes_sent);

  if (wc < 0)
    return handle_error(*state, INTERNAL_SERVER_ERROR, CGI_DATA_SEND_FAILURE,
                        server);

  if (wc == (long)state->cgi_buff.size() - state->bytes_sent) {
    close(state->cgi_pipe_w[1]);
    state->cgi_pipe_w[1] = 0;
    state->bytes_sent = 0;
    state->cgi_buff = std::string();
    if (!exe_cgi(*state, server))
      return;
    wait_to_read_cgi(*state, server);
  } else {
    state->bytes_sent += wc;
  }
}