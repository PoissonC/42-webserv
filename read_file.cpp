/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/21 20:21:44 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;
  char buf[BUFFER_SIZE];
  ssize_t rc = read(state->file_fd, buf, BUFFER_SIZE);
  
  if (rc < 0) {
    // TODO: handle error
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
    return;
  }

  state->file_buff += buf;

  // end of reading
  if (rc < BUFFER_SIZE) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    // TODO: Generate the response along with the file content
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
  }
}