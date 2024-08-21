/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPublicMemberFuncs.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:08:17 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/21 17:04:07 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::vector<struct pollfd>::iterator Server::getNextPfdsEnd() {
  return _next_poll_fds.end();
}

void Server::add_to_poll_in(int fd) {
  struct pollfd new_pfd;
  bzero(&new_pfd, sizeof(struct pollfd));
  new_pfd.events = POLLIN | POLLHUP | POLLERR;
  new_pfd.fd = fd;
  _next_poll_fds.push_back(new_pfd);
}

void Server::add_to_poll_out(int fd) {
  struct pollfd new_pfd;
  bzero(&new_pfd, sizeof(struct pollfd));
  new_pfd.events = POLLOUT | POLLHUP | POLLERR;
  new_pfd.fd = fd;
  _next_poll_fds.push_back(new_pfd);
}


void Server::close_conn(int fd, std::vector<State>::iterator &cur_state) {
  close(fd);
  for (std::vector<struct pollfd>::iterator it = _next_poll_fds.begin();
       it != _next_poll_fds.end(); it++) {
    if (it->fd == fd) {
      _next_poll_fds.erase(it);
      break;
    }
  }
  // This should destroy the cur_state element (free resources, ChatGPT says so)
  _states.erase(cur_state);
  std::cout << "connection ends" << std::endl;
}

std::vector<struct pollfd>::iterator Server::find_it_in_nxt(int fd) {
  for (std::vector<struct pollfd>::iterator it = _next_poll_fds.begin();
       it != _next_poll_fds.end(); it++) {
    if (it->fd == fd)
      return it;
  }
  return _next_poll_fds.end();
}

ServerConfig & Server::getServerConfig(State & state) {
  for(std::vector<Settings>::iterator settings_it = _settings.begin(); settings_it != settings_it.end(); settings_it++) {

  }
}

char** Server::get_env() {
  return _env;
}


void Server::remove_from_poll(int fd) {
  _next_poll_fds.erase(find_it_in_nxt(fd));
}


Server::Server(std::vector<Settings> &settings, char **env) : _settings(settings), _env(env) {
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
