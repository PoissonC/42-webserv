/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 22:00:02 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/26 18:54:27 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Settings.hpp"
#include "State.hpp"
#include "MiddleStages.hpp"
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

#define BUFFER_SIZE 10
#define BACK_LOG 32
#define MAX_EVENTS 16

class Server {

public:
  // Constructer, it creates the non-blocking socket connection, listen to the
  // ip/port from settings 	calling socket, setsockopt, bind, listen
  //   put socket fds into pfs
  Server(std::vector<Settings> &servers, char **env);
  ~Server();

  // getters
  std::vector<struct pollfd>::iterator getNextPfdsEnd();
  char** get_env();
  // member methods
  void close_conn(int fd, std::vector<State>::iterator &cur_state);
  void new_conns(int sock_fd);
  void add_to_poll_in(int fd);
  void add_to_poll_out(int fd);
  std::vector< struct pollfd >::iterator find_it_in_nxt(int fd);
  void getServerConfig(State &);
  void remove_from_poll(int fd);
  

  //	Start waiting for events
  //		calling poll, accept, recv, send
  void run();

private:
  char **_env;
  static bool _constructed;
  std::vector<int> _socks_fd;
  std::vector<struct pollfd> _cur_poll_fds;
  std::vector<struct pollfd> _next_poll_fds;
  std::vector<Settings> &_settings;
  std::vector<State> _states;

  // funcs

  bool is_socket(int fd);
  std::vector<State>::iterator getState(int fd);

  void run_a_server(
      std::vector<Settings>::iterator &); // socket, setsockopt, bind, listen
};
