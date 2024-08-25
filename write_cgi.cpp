/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 18:53:42 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "handle_error_response.hpp"

void write_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;
  if (state->cgi_pipe_w[1] != pfd.fd) {
    throw std::runtime_error("Control Flow is Wrong\n");
    return;
  }

  ssize_t wc = send(pfd.fd, state->cgi_buff.c_str() + state->bytes_sent,
                    state->cgi_buff.size() - state->bytes_sent, MSG_DONTWAIT);

  if (wc == (long)state->response_buff.size() - state->bytes_sent) {
    state->stage = &read_request;
    poll_to_in(state->conn_fd, server);
    state->reset_attrs();
  } else {
    state->bytes_sent += wc;
  }
}