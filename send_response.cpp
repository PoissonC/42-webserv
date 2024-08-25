/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 21:03:59 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void send_response(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;

  ssize_t wc = send(state->conn_fd, state->response_buff.c_str() + state->bytes_sent,
                    state->response_buff.size() - state->bytes_sent, MSG_DONTWAIT);

  if (wc == (long)state->response_buff.size() - state->bytes_sent) {
    state->stage = &read_request;
    poll_to_in(state->conn_fd, server);
    state->reset_attrs();
  } else {
    state->bytes_sent += wc;
  }
}