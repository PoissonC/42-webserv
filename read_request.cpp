/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:29 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 22:22:20 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Server;

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "handle_error_response.hpp"

void read_request(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;

  char buf[BUFFER_SIZE];
  bzero(buf, BUFFER_SIZE);
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
    state->req = Request(state->request_buff);
    int status_code = state->req.checkRequest();
    if (status_code != 200) {
      // TODO: handle error
      handle_error_response(*state, status_code, "Bad request.", server);
      return;
    }
	  handle_request(*state, server);
  }
}
