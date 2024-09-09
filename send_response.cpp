/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhurpy <jhurpy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/09 14:40:08 by jhurpy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "constants.hpp"

void send_response(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;

  ssize_t wc = send(state->conn_fd, state->response_buff.c_str() + state->bytes_sent,
                    state->response_buff.size() - state->bytes_sent, MSG_DONTWAIT);

  if (wc < 0) {
    std::cerr << "Close conn by send_response" << std::endl;
    server.close_conn(state);
    return;
  }

  if (wc == (long)state->response_buff.size() - state->bytes_sent) {
    state->stage = &read_request;
    poll_to_in(state->conn_fd, server);
    if (state->res.getStatusCode() == BAD_REQUEST) {
      std::cerr << "Close conn by send_response, bad request" << std::endl;
      server.close_conn(state);
    }
    else
      state->reset_attrs();
  } else {
    state->bytes_sent += wc;
  }
}
