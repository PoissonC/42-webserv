/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 18:43:02 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "handle_error_response.hpp"

void read_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;
  char buf[BUFFER_SIZE];
  bzero(buf, sizeof(buf));

  ssize_t rc = read(state->cgi_pipe_r[0], buf, BUFFER_SIZE);
  
  if (rc < 0) {
    // TODO: handle error
    close(state->cgi_pipe_r[0]);
    server.remove_from_poll(state->cgi_pipe_r[0]);
    handle_error_response(*state, 500, "Read CGI output failed.", server);
    return;
  }

  state->cgi_buff += buf;

  // end of reading
  if (rc < BUFFER_SIZE) {
    close(state->cgi_pipe_r[0]);
    server.remove_from_poll(state->cgi_pipe_r[0]);
    wait_to_send_resonpse(*state, server);
  }
}