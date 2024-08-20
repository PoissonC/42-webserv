/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:29 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/20 19:07:28 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Server;

#include "MiddleStages.hpp"

void read_request(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;

  char buf[BUFFER_SIZE];
  ssize_t rc = recv(state->conn_fd, buf, BUFFER_SIZE, MSG_DONTWAIT);

  // < 0 ..> an error occurs, = 0 client closes the connection
  if (rc <= 0) {
    server.close_conn(state->conn_fd, state);
    return;
  }

  state->request_buff += buf;

  if (rc < BUFFER_SIZE) {
    // finish reading, it needs to do something and checks conditions. Below
    // just for tests:
    state->req = Request(buf);
    state->stage = &send_response;
    std::vector<struct pollfd>::iterator next_pfd =
        server.find_it_in_nxt(pfd.fd);
    next_pfd->events = POLLOUT | POLLHUP | POLLERR;
  }
}
