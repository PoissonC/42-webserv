/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/26 15:12:27 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "LocationConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "MiddleStages.hpp"
#include "ServerConfig.hpp"

class State {
public:
  State(int fd, unsigned char * client_addr, int socket);

  void reset_attrs();
  void setup_cgi();

  std::string request_buff;
  std::string file_buff;
  std::string cgi_buff;

  std::string response_buff;
  long bytes_sent;

  std::string original_path; // to call open
  std::string file_path; // to call open
  std::string cgi_path; // to call execve

  Request req;
  Response res;

  unsigned char *client_ip;

  int cgi_pipe_r[2], cgi_pipe_w[2];
  int sock_fd, conn_fd, file_fd;
  void (*stage)(std::vector<State>::iterator &, const struct pollfd &, Server &);


  ServerConfig  server;
  LocationConfig  loc;
};
