/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/20 19:25:17 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "MiddleStages.hpp"

class State {
public:
  State();

  std::string request_buff;
  std::string response_buff;
  std::string file_buff;

  Request req;
  Response res;

  unsigned char *client_ip;

  int cgi_pipe[2];
  int conn_fd, file_fd;
  void (*stage)(std::vector<State>::iterator &, const struct pollfd &, Server &);

};
