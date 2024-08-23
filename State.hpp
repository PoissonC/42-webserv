/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/21 20:03:07 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "MiddleStages.hpp"

class State {
public:
  State(int fd, unsigned char * client_addr, int socket);

  std::string request_buff;
  std::string file_buff;

  std::string response_buff;
  long bytes_sent;

  std::string file_path; // to call open
  std::string cgi_path; // to call execve

  Request req;
  Response res;

  unsigned char *client_ip;

  int cgi_pipe[2];
  int sock_fd, conn_fd, file_fd;
  void (*stage)(std::vector<State>::iterator &, const struct pollfd &, Server &);

};
