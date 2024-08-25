/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:29 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 19:19:46 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Server;

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void read_request(std::vector<State>::iterator &state, const struct pollfd &pfd,
                  Server &server) {
  if (!(pfd.revents & POLLIN))
    return;

  char buf[BUFFER_SIZE];
  bzero(buf, BUFFER_SIZE);
  ssize_t rc = recv(state->conn_fd, buf, BUFFER_SIZE, MSG_DONTWAIT);

  // < 0 ..> an error occurs, = 0 client closes the connection
  if (rc <= 0) {
    std::cout << "close connection from read_request." << std::endl;
    server.close_conn(state->conn_fd, state);
    return;
  }

  state->request_buff += buf;

  if (rc < BUFFER_SIZE) {
    // finish reading, it needs to do something and checks conditions. Below
    // just for tests:
    state->req = Request(state->request_buff);
    state->res.setStatus(state->req.checkRequest());
    if (state->res.getStatusCode() != 200) {
      // TODO: handle error
      state->stage = &send_response;
      poll_to_out(state->conn_fd, server);
      return;
    }
    select_loc_config(*state, server);

    // state->stage = &send_response;
    // poll_to_out(state->conn_fd, server);
    // return;
    // above needs to be removed

    /*
      TODO: check the config file and decide which case to handle
      Jeremy's job
    */
    // Function will compare the request with the server config
    // func(serverConfig);
  }
}
