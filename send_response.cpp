/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/21 16:55:12 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"

void send_response(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;

  // For test:
  state->response_buff = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 13\n\nHello World!\n";
  // 
  ssize_t wc = send(state->conn_fd, state->response_buff.c_str() + state->bytes_sent,
                    state->response_buff.size() - state->bytes_sent, MSG_DONTWAIT);

  if (wc == (long)state->response_buff.size() - state->bytes_sent) {
    state->stage = &read_request;
    std::vector<struct pollfd>::iterator next_pfd =
        server.find_it_in_nxt(pfd.fd);
    next_pfd->events = POLLIN | POLLHUP | POLLERR;
    std::cout << state->response_buff << std::endl;
  } else {
    state->bytes_sent += wc;
  }

}