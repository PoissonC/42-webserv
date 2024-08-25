/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/25 21:34:42 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"

State::State(int fd, unsigned char * client_addr, int socket): bytes_sent(0), req(std::string()), client_ip(client_addr), sock_fd(socket) , conn_fd(fd), stage(&read_request) {
  bzero(cgi_pipe_r, sizeof(cgi_pipe_r));
  bzero(cgi_pipe_w, sizeof(cgi_pipe_w));
  std::cout << "New connection fd: " << fd << std::endl;
}

void State::reset_attrs() {
  cgi_buff = std::string();
  request_buff = std::string();
  file_buff = std::string();
  response_buff = std::string();
  original_path = std::string();
  file_path = std::string();
  cgi_path = std::string();
  bytes_sent = 0;
  server = ServerConfig();
  loc = LocationConfig();
  request_buff = std::string();
  res = Response();
}
