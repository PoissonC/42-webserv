/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPublicMemberFuncs.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:08:17 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/28 17:45:45 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
#include <csignal>

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
  if (close(fd))
    return;
  for (std::vector<struct pollfd>::iterator it = _next_poll_fds.begin();
       it != _next_poll_fds.end(); it++) {
    if (it->fd == fd) {
      _next_poll_fds.erase(it);
      break;
    }
  }
  // This should destroy the cur_state element (free resources, ChatGPT says so)
  _states.erase(cur_state);
}

std::vector<struct pollfd>::iterator Server::find_it_in_nxt(int fd) {
  for (std::vector<struct pollfd>::iterator it = _next_poll_fds.begin();
       it != _next_poll_fds.end(); it++) {
    if (it->fd == fd)
      return it;
  }
  return _next_poll_fds.end();
}

void Server::getServerConfig(State &state) {
  std::map<std::string, std::string> headers = state.req.getHeaders();
  std::map<std::string, std::string>::iterator host_it = headers.find("Host");
  std::string host;
  if (host_it != headers.end())
    host = host_it->second;
  for (std::vector<Settings>::iterator settings_it = _settings.begin();
       settings_it != _settings.end(); settings_it++) {
    if (state.sock_fd == settings_it->_socket_fd) {
      if (host.empty()) {
        state.server = *(settings_it->_servers.begin());
        return;
      }
      for (std::vector<ServerConfig>::iterator server_it =
               settings_it->_servers.begin();
           server_it != settings_it->_servers.end(); server_it++) {
        std::vector<std::string> server_names = server_it->getServerNames();
        for (std::vector<std::string>::iterator server_name =
                 server_names.begin();
             server_name != server_names.end(); server_name++) {
          if (host == *server_name) {
            state.server = *server_it;
            return;
          }
        }
      }
      state.server = *(settings_it->_servers.begin());
      return;
    }
  }
}

char **Server::get_env() { return _env; }

void Server::remove_from_poll(int fd) {
  std::vector<pollfd>::iterator nxt = find_it_in_nxt(fd);
  if (nxt == _next_poll_fds.end())
    return;
  _next_poll_fds.erase(find_it_in_nxt(fd));
}

Server::Server(std::vector<Settings> &settings, char **env)
    : _env(env), _settings(settings) {
  if (_constructed)
    throw std::runtime_error(SERVER_DUPLICATE);
  _constructed = true;
  signal(SIGCHLD, SIG_IGN);

  // Set socket for all servers, bind and listen.
  for (std::vector<Settings>::iterator it = _settings.begin();
       it != _settings.end(); it++)
    run_a_server(it);
}

void Server::run() {
  while (1) {
    _cur_poll_fds = _next_poll_fds;
    checkTimeoutCGI();
    int nfds = poll(_cur_poll_fds.data(), _cur_poll_fds.size(), SERVER_TIMEOUT);
    if (nfds == -1)
      throw std::runtime_error(strerror(errno));

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
          throw std::runtime_error(STATE_NOT_FOUND);

        if (it->revents & (POLLHUP | POLLERR)) {
          // Close connection on error (http/1.1 keeps the connection)
          if (it->fd == cur_state->conn_fd)
            close_conn(it->fd, cur_state);
          else if (it->fd == cur_state->file_fd)
            handle_error(*cur_state, UNDEFINED, READ_FILE_FAILURE, *this);
          else if (it->revents & (POLLHUP)) {
            cur_state->stage(cur_state, *it, *this);
          } else
            handle_error(*cur_state, UNDEFINED, READ_CGI_FAILURE, *this);
          continue;
        }

        cur_state->stage(cur_state, *it, *this);
      }
    }
  }
}
