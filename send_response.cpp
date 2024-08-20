/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/20 19:21:49 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"

void send_response(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;

  static int resStringSentBytes = 0;
  std::string resString = state->res.generateResponseString();
  e_methods method = state->req.getMethod();
  std::string uri = state->req.getUri();

  ssize_t wc = send(state->conn_fd, resString.c_str() + resStringSentBytes,
                    resString.size() - resStringSentBytes, MSG_DONTWAIT);

  if (wc == (long)resString.size() - resStringSentBytes) {
    // finish reading, it needs to do something and checks conditions. Below
    // just for tests:
    state->stage = &read_request;
    std::vector<struct pollfd>::iterator next_pfd =
        server.find_it_in_nxt(pfd.fd);
    next_pfd->events = POLLIN | POLLHUP | POLLERR;
    std::cout << resString << std::endl;
  } else {
    resStringSentBytes += wc;
  }

}