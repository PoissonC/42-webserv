/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_helper.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 21:32:14 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/07 22:19:35 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server_helper.hpp"


void close_fds(std::vector<int> &fds) {
  for (std::vector<int>::iterator it = fds.begin(); it != fds.end(); it++) {
    close(*it);
  }
}

void poll_to_out(int fd, Server & server) {
  std::vector<struct pollfd>::iterator next_pfd =
      server.find_it_in_nxt(fd);
  // if the fd is not in the poll array
  if (next_pfd == server.getNextPfdsEnd()) {
    server.add_to_poll_out(fd);
    return;
  }
  // else
  next_pfd->events = POLLOUT;
}

void poll_to_in(int fd, Server & server) {
  std::vector<struct pollfd>::iterator next_pfd =
      server.find_it_in_nxt(fd);
  // if the fd is not in the poll array
  if (next_pfd == server.getNextPfdsEnd()) {
    server.add_to_poll_in(fd);
    return;
  }
  // else
  next_pfd->events = POLLIN;
}

void wait_to_save_file(State &state, Server & server) {
  state.stage = &save_file;
  server.remove_from_poll(state.conn_fd);
  poll_to_out(state.file_fd, server);
}

void wait_to_read_file(State &state, Server & server) {
  state.stage = &read_file;
  server.remove_from_poll(state.conn_fd);
  poll_to_in(state.file_fd, server);
}

void wait_to_read_cgi(State &state, Server & server) {
  state.stage = &read_cgi;
  server.remove_from_poll(state.conn_fd);
  poll_to_in(state.cgi_pipe_r[0], server);
}

void wait_to_write_cgi(State &state, Server & server) {
  state.bytes_sent = 0;
  state.stage = &write_cgi;
  server.remove_from_poll(state.conn_fd);
  poll_to_out(state.cgi_pipe_w[1], server);
}

void wait_to_send_resonpse(State &state, Server & server) {
  state.stage = &send_response;
  poll_to_out(state.conn_fd, server);
  state.bytes_sent = 0;
  state.response_buff = state.res.generateResponseString();
}