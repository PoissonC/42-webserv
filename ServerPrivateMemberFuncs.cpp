/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPrivateMemberFuncs.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:05:27 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/28 17:39:06 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Server_helper.hpp"
#include "handle_error_response.hpp"
#include <signal.h>

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
  add_to_poll_in(new_socket_fd);
  // std::cout << "Start server for:\n" ;
  // for (std::vector<ServerConfig>::iterator sit = it->_servers.begin(); sit != it->_servers.end(); sit++) {
  //   std::vector<std::string> servernames = sit->getServerNames();
  //   for (std::vector<std::string>::iterator snit = servernames.begin(); snit != servernames.end(); snit++ )
  //     std::cout << *snit;
  //   std::cout << std::endl;
  // }
}

void Server::new_conns(int sock_fd) {
  int new_sd;
  // Accept new connections
  struct sockaddr_in addr_client;
  socklen_t client_addr_len = sizeof(addr_client);
  while ((new_sd = accept(sock_fd, (sockaddr *)&addr_client,
                          &client_addr_len)) != -1) {
    add_to_poll_in(new_sd);

    _states.push_back(State(new_sd, ntohl(addr_client.sin_addr.s_addr), sock_fd));
  }
}

bool Server::is_socket(int fd) {
  return std::find(_socks_fd.begin(), _socks_fd.end(), fd) == _socks_fd.end() ? false : true;
}

std::vector<State>::iterator Server::getState(int fd) {
  for (std::vector<State>::iterator it = _states.begin(); it != _states.end();
       it++) {
    if (it->conn_fd == fd || it->file_fd == fd || it->cgi_pipe_w[1] == fd || it->cgi_pipe_r[0] == fd)
      return it;
  }
  return _states.end();
}

void  Server::checkTimeouotCGI() {
  for (std::vector<struct pollfd>::iterator it = _cur_poll_fds.begin(); it != _cur_poll_fds.end(); it++) {
    std::vector<State>::iterator cur_state = getState(it->fd);
    if (cur_state == _states.end())
      continue;
    if (!cur_state->isCGIrunning)
      continue;
    if (std::time(NULL) - cur_state->timeCGI > CGI_TIMEOUT) {
      if (cur_state->cgiPID > 0)
        kill(cur_state->cgiPID, SIGKILL);
      handle_error_response(*cur_state, 502, "Executing CGI script failed, waiting timeout.", *this);
    }
  }
  _cur_poll_fds = _next_poll_fds;
}


Server::~Server() { close_fds(_socks_fd); }
