/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPrivateMemberFuncs.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:05:27 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/20 19:25:34 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Server_helper.hpp"

bool Server::_constructed = false;

void Server::run_a_server(std::vector<Settings>::iterator &it) {
  int new_socket_fd;
#ifdef __linux__
  new_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  // For Linux, set the socket to non-blocking at the time of creation
  if (new_socket_fd < 0) {
    throw std::runtime_error("socket failed");
  }
#elif __APPLE__
  // For macOS, set the socket to non-blocking using fcntl
  new_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (new_socket_fd < 0) {
    throw std::runtime_error("socket failed");
  }

  int flags = fcntl(new_socket_fd, F_GETFL, 0);
  if (flags == -1) {
    close_fds(_socks_fd);
    throw std::runtime_error("fcntl failed");
  }
  if (fcntl(new_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    close_fds(_socks_fd);
    throw std::runtime_error("fcntl failed");
  }
#endif

  it->_socket_fd = new_socket_fd;
  _socks_fd.push_back(new_socket_fd);

  // To allow address receive multiple connections at the same time
  {
    int on = 1;
    if (setsockopt(new_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,
                   sizeof(on)) < 0) {
      close_fds(_socks_fd);
      throw std::runtime_error("setsockopt failed");
    }
  }

  if (bind(new_socket_fd, (struct sockaddr *)&(it->_addr), sizeof(it->_addr)) <
      0) {
    close_fds(_socks_fd);
    throw std::runtime_error("bind failed");
  }

  if (listen(new_socket_fd, BACK_LOG) < 0) {
    close_fds(_socks_fd);
    throw std::runtime_error("listen failed");
  }

  // Setting up sockets for poll
  add_to_poll(new_socket_fd);
}

void Server::new_conns(int sock_fd) {
  int new_sd;
  // Accept new connections
  struct sockaddr_in addr_client;
  socklen_t client_addr_len = sizeof(addr_client);
  while ((new_sd = accept(sock_fd, (sockaddr *)&addr_client,
                          &client_addr_len)) != -1) {
    struct pollfd new_pfd;
    bzero(&new_pfd, sizeof(struct pollfd));
    new_pfd.fd = new_sd;
    new_pfd.events = POLLIN | POLLHUP | POLLERR;
    _next_poll_fds.push_back(new_pfd);

    State new_conn;
    // not finished
    bzero(&new_conn, sizeof(State));
    new_conn.stage = &read_request;
    new_conn.conn_fd = new_sd;
    new_conn.client_ip = (unsigned char *)&addr_client.sin_addr.s_addr;
    _states.push_back(new_conn);
    std::cout << "New connection fd: " << new_conn.conn_fd << std::endl;
  }
}

bool Server::is_socket(int fd) {
  return std::find(_socks_fd.begin(), _socks_fd.end(), fd) == _socks_fd.end() ? false : true;
}

std::vector<State>::iterator Server::getState(int fd) {
  for (std::vector<State>::iterator it = _states.begin(); it != _states.end();
       it++) {
    if (it->conn_fd == fd || it->file_fd == fd || it->cgi_pipe[1] == fd)
      return it;
  }
  return _states.end();
}


Server::Server(std::vector<Settings> &settings) : _settings(settings) {
  if (_constructed)
    throw std::runtime_error("The server instance already exists");
  _constructed = true;

  // set socket for all servers, bind and listen.
  for (std::vector<Settings>::iterator it = _settings.begin();
       it != _settings.end(); it++)
    run_a_server(it);
}

void Server::run() {
  while (1) {
    _cur_poll_fds = _next_poll_fds;
    int nfds = poll(_cur_poll_fds.data(), _cur_poll_fds.size(), -1);
    if (nfds == -1)
      throw std::runtime_error("poll failed");

    // Process all returned events
    for (std::vector<struct pollfd>::iterator it = _cur_poll_fds.begin();
         it != _cur_poll_fds.end(); it++) {
      if (it->revents == 0)
        continue;
      // Check if the event is for a server socket
      if (is_socket(it->fd))
        new_conns(it->fd);

      // If the event is not in _socks_fd, handle 5 stages
      else {
        std::vector<State>::iterator cur_state = getState(it->fd);

        if (cur_state == _states.end())
          throw std::runtime_error("State not found");

        // Close connection if any error occurs (http/1.1 keeps the connection)
        if (it->revents & (POLLHUP | POLLERR)) {
          close_conn(it->fd, cur_state);
          continue;
        }
        // if (it->revents & POLLIN)
        // 	std::cout << "Input ready." << std::endl;
        // if (it->revents & POLLOUT)
        // 	std::cout << "Output ready." << std::endl;
        cur_state->stage(cur_state, *it, *this);

      }
    }
  }
}

Server::~Server() { close_fds(_socks_fd); }
