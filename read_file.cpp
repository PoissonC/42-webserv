/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 21:43:42 by ychen2           ###   ########.fr       */
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
  ssize_t rc = read(state->file_fd, buf, BUFFER_SIZE);
  
  if (rc < 0) {
    // TODO: handle error
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    // The second time error occurs
    if (state->res.getStatusCode() != 200) {
      state->stage = &send_response;
      state->response_buff = state->res.generateResponseString();
      poll_to_out(state->conn_fd, server);
      return;
    }
    handle_error_response(*state, 500, "Read file failed.", server);
    return;
  }
    std::cout << "Reading...." << std::endl;

  state->file_buff += buf;
  // end of reading
  if (rc < BUFFER_SIZE) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    // TODO: Generate the response along with the file content
    state->res.setBody(state->file_buff);
    state->response_buff = state->res.generateResponseString();
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
    state->file_buff = std::string();
  }
}