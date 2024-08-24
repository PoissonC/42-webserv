/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 20:20:10 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void send_response(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;

  // For test:
  // state->response_buff = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 13\n\nHello World!\n";
  // 
  ssize_t wc = send(state->conn_fd, state->response_buff.c_str() + state->bytes_sent,
                    state->response_buff.size() - state->bytes_sent, MSG_DONTWAIT);

  if (wc == (long)state->response_buff.size() - state->bytes_sent) {
    state->stage = &read_request;
    poll_to_in(state->conn_fd, server);
    state->bytes_sent = 0;
    state->request_buff = std::string();
  } else {
    state->bytes_sent += wc;
  }
}