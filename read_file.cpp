/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/26 18:56:23 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "handle_error_response.hpp"

void  read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;
  char buf[BUFFER_SIZE];
  bzero(buf, BUFFER_SIZE);
  ssize_t rc = read(state->file_fd, buf, BUFFER_SIZE - 1);
  
  if (rc < 0) {
    // TODO: handle error
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    // The second time error occurs
    if (state->res.getStatusCode() != 200) {
      wait_to_send_resonpse(*state, server);
      return;
    }
    handle_error_response(*state, 500, "Read file failed.", server);
    return;
  }
    std::cout << "Reading...." << std::endl;

  state->file_buff += buf;
  // end of reading
  if (rc < BUFFER_SIZE - 1) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    // TODO: Generate the response along with the file content
    state->res.setBody(state->file_buff);
    state->file_buff = std::string();
    wait_to_send_resonpse(*state, server);
  }
}