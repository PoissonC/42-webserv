/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/09 18:53:58 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "constants.hpp"
#include "handle_error.hpp"

void read_file(std::list<State>::iterator &state, const struct pollfd &pfd,
               Server &server) {
  if (!(pfd.revents & POLLIN))
    return;
  char buf[BUFFER_SIZE];
  ssize_t rc = read(state->file_fd, buf, BUFFER_SIZE);

  if (rc < 0) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    state->file_fd = 0;
    if (state->res.getStatusCode() != OK) {
      wait_to_send_resonpse(*state, server);
      return;
    }
    return handle_error(*state, UNDEFINED, READ_FILE_FAILURE, server);
  }

  state->file_buff.append(buf, rc);

  if (rc < BUFFER_SIZE) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    state->file_fd = 0;
    state->res.setBody(state->file_buff);
    if (state->res.getBody().empty())
      state->res.setStatusCode(NO_CONTENT);
    state->file_buff = std::string();
    wait_to_send_resonpse(*state, server);
  }
}