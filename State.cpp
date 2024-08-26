/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/26 16:32:20 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"

State::State(int fd, uint32_t client_addr, int socket): bytes_sent(0), req(std::string()), sock_fd(socket) , conn_fd(fd), stage(&read_request) {
  std::ostringstream ip_stream;
  ip_stream << ((client_addr >> 24) & 0xFF) << '.'
            << ((client_addr >> 16) & 0xFF) << '.'
            << ((client_addr >> 8) & 0xFF) << '.'
            << (client_addr & 0xFF);
  
  client_ip_str = ip_stream.str();
  std::cout << "Client IP: " << client_ip_str << std::endl;
  bzero(cgi_pipe_r, sizeof(cgi_pipe_r));
  bzero(cgi_pipe_w, sizeof(cgi_pipe_w));
  std::cout << "New connection fd: " << fd << std::endl;
}

void State::reset_attrs() {
  bzero(cgi_pipe_r, sizeof(cgi_pipe_r));
  bzero(cgi_pipe_w, sizeof(cgi_pipe_w));
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
