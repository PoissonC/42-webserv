/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/20 21:34:11 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void read_cgi(std::vector<State>::iterator & state, const struct pollfd & pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;
  char buf[BUFFER_SIZE];
  ssize_t rc = read(state->cgi_pipe[0], buf, BUFFER_SIZE);
  
  if (rc < 0) {
    // TODO: handle error
    wait_to_respond(state, pfd, server);
    return;
  }

  state->response_buff += buf;

  // end of reading
  if (rc < BUFFER_SIZE) {
    close(state->file_fd);
    wait_to_respond(state, pfd, server);
  }
}