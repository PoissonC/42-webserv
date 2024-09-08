/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPrivateMemberFuncs.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:05:27 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/08 21:20:08 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Server_helper.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
#include <csignal>

bool Server::_constructed = false;

void Server::run_a_server(std::vector<Settings>::iterator &it) {
  int new_socket_fd;
#ifdef __linux__
  // new_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  new_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  // For Linux, set the socket to non-blocking at the time of creation
  if (new_socket_fd < 0) {
    throw std::runtime_error(strerror(errno));
  }
#elif __APPLE__
  // For macOS, set the socket to non-blocking using fcntl
  new_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (new_socket_fd < 0)
    throw std::runtime_error(strerror(errno));

  int flags = fcntl(new_socket_fd, F_GETFL, 0);
  if (flags == -1) {
    close_fds(_socks_fd);
    throw std::runtime_error(strerror(errno));
  }
  if (fcntl(new_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    close_fds(_socks_fd);
    throw std::runtime_error(strerror(errno));
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
      throw std::runtime_error(strerror(errno));
    }
  }

  if (bind(new_socket_fd, (struct sockaddr *)&(it->_addr), sizeof(it->_addr)) <
      0) {
    close_fds(_socks_fd);
    throw std::runtime_error(strerror(errno));
  }

  if (listen(new_socket_fd, BACK_LOG) < 0) {
    close_fds(_socks_fd);
    throw std::runtime_error(strerror(errno));
  }

  // Set up sockets for poll
  add_to_poll_in(new_socket_fd);
}

void Server::new_conns(int sock_fd) {
  int new_sd;

  // Accept new connections
  struct sockaddr_in addr_client;
  socklen_t client_addr_len = sizeof(addr_client);
  if ((new_sd = accept(sock_fd, (sockaddr *)&addr_client, &client_addr_len)) < 0)
    throw std::runtime_error("Poll doesn't work.");
  add_to_poll_in(new_sd);
  _states.push_back(
      State(new_sd, ntohl(addr_client.sin_addr.s_addr), sock_fd));
}

bool Server::is_socket(int fd) {
  return !(std::find(_socks_fd.begin(), _socks_fd.end(), fd) ==
           _socks_fd.end());
}

std::list<State>::iterator Server::getState(int fd) {
  for (std::list<State>::iterator it = _states.begin(); it != _states.end();
       it++) {
    if (it->conn_fd == fd || it->file_fd == fd || it->cgi_pipe_w[1] == fd ||
        it->cgi_pipe_r[0] == fd)
      return it;
  }
  return _states.end();
}

void Server::checkTimeoutConn() {
  bool isDelete = true;
  while(isDelete) {
    isDelete = false;
    for (std::list<State>::iterator it = _states.begin(); it != _states.end(); it++) {
      if (std::time(NULL) - it->timeEvent > EVENT_TIMEOUT) {
      std::cerr << "Close conn by checkTimeout" << std::endl;
        close_conn(it);
        isDelete = true;
        break;
      }
    }
  }
  _cur_poll_fds = _next_poll_fds;
}


void Server::checkTimeoutCGI() {
  for (std::vector<struct pollfd>::iterator it = _cur_poll_fds.begin();
       it != _cur_poll_fds.end(); it++) {
    std::list<State>::iterator cur_state = getState(it->fd);
    if (cur_state == _states.end())
      continue;
    if (!cur_state->isCGIrunning)
      continue;
    if (std::time(NULL) - cur_state->timeCGI > CGI_TIMEOUT) {
      if (cur_state->cgiPID > 0)
        kill(cur_state->cgiPID, SIGKILL);
      cur_state->isCGIrunning = false;
      handle_error(*cur_state, UNDEFINED, TIMEOUT_ERROR, *this);
    }
  }
  _cur_poll_fds = _next_poll_fds;
}

Server::~Server() { close_fds(_socks_fd); }
