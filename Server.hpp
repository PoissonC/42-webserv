/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 22:00:02 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/18 16:34:36 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Settings.hpp"
#include "State.hpp"
#include <algorithm>
#include <cstdio>
#include <exception>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <utility>
#include <vector>
#ifdef __APPLE__
#include <fcntl.h>
#endif

#define BUFFER_SIZE 1024
#define BACK_LOG 32
#define MAX_EVENTS 16

class Server {

public:
  // Constructer, it creates the non-blocking socket connection, listen to the
  // ip/port from settings 	calling socket, setsockopt, bind, listen
  //   put socket fds into pfs
  Server(std::vector<Settings> &servers);
  ~Server();
  // member methods

  //	Start waiting for events
  //		calling poll, accept, recv, send
  void run();

private:
  static bool _constructed;
  std::vector<int> _socks_fd;
  std::vector<struct pollfd> _cur_poll_fds;
  std::vector<struct pollfd> _next_poll_fds;
  std::vector<Settings> &_settings;
  std::vector<t_state> _states;

  // Stages would block
  void
  read_request(std::vector<t_state>::iterator &state,
               const struct pollfd &pfd); // open the requested file in the end
  void send_response(std::vector<t_state>::iterator &state,
                     const struct pollfd &pfd);
  static void read_file(std::vector<t_state>::iterator &state,
                        const struct pollfd &pfd);
  void fork_cgi(std::vector<t_state>::iterator &state,
                const struct pollfd &pfd);
  void read_cgi(std::vector<t_state>::iterator &state,
                const struct pollfd &pfd);

  // funcs
  void new_conn_stage(std::vector<t_state>::iterator &state,
                      const struct pollfd &pfd);
  void run_a_server(
      std::vector<Settings>::iterator &); // socket, setsockopt, bind, listen
};
