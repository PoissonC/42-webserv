/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/20 20:46:18 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void read_file(std::vector<State>::iterator & state, const struct pollfd & pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;
  char buf[BUFFER_SIZE];
  ssize_t rc = read(state->conn_fd, buf, BUFFER_SIZE);
  
  if (rc < 0) {
    // TODO: handle error
    wait_to_respond(state, pfd, server);
    return;
  }

  state->file_buff += buf;

  // end of reading
  if (rc < BUFFER_SIZE) {
    close(state->file_fd);
    // TODO: Generate the response along with the file content
    wait_to_respond(state, pfd, server);
  }
}