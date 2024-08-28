/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/28 16:14:00 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "LocationConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "MiddleStages.hpp"
#include "ServerConfig.hpp"
#include <stdint.h>
#include <ctime>

class State {
public:
  State(int fd, uint32_t client_addr, int socket);

  void reset_attrs();
  void setup_cgi();

  pid_t       cgiPID;
  size_t      bodyPos;
  size_t      contentLength;
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

  std::string client_ip_str;

  int cgi_pipe_r[2], cgi_pipe_w[2];
  int sock_fd, conn_fd, file_fd;
  void (*stage)(std::vector<State>::iterator &, const struct pollfd &, Server &);


  ServerConfig  server;
  LocationConfig  loc;

  std::time_t timeCGI;
  bool        isCGIrunning;
};
