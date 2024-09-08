/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPublicMemberFuncs.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:08:17 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/08 18:19:40 by ychen2           ###   ########.fr       */
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
  new_pfd.events = POLLIN;
  new_pfd.fd = fd;
  _next_poll_fds.push_back(new_pfd);
}

void Server::add_to_poll_out(int fd) {
  struct pollfd new_pfd;
  bzero(&new_pfd, sizeof(struct pollfd));
  new_pfd.events = POLLOUT;
  new_pfd.fd = fd;
  _next_poll_fds.push_back(new_pfd);
}

void Server::close_conn(std::list<State>::iterator &cur_state) {
  if (cur_state->conn_fd != 0)
    close(cur_state->conn_fd);
  if (cur_state->file_fd != 0)
    close(cur_state->file_fd);
  if (cur_state->cgi_pipe_r[0] != 0)
    close(cur_state->cgi_pipe_r[0]);
  if (cur_state->cgi_pipe_w[1] != 0)
    close(cur_state->cgi_pipe_w[1]);
  std::vector<struct pollfd>::iterator it;
  it = find_it_in_nxt(cur_state->conn_fd);
  if (it != _next_poll_fds.end())
    _next_poll_fds.erase(it);
  it = find_it_in_nxt(cur_state->file_fd);
  if (it != _next_poll_fds.end())
    _next_poll_fds.erase(it);
  it = find_it_in_nxt(cur_state->cgi_pipe_r[0]);
  if (it != _next_poll_fds.end())
    _next_poll_fds.erase(it);
  it = find_it_in_nxt(cur_state->cgi_pipe_w[1]);
  if (it != _next_poll_fds.end())
    _next_poll_fds.erase(it);

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
    checkTimeoutConn();
    int nfds = poll(_cur_poll_fds.data(), _cur_poll_fds.size(), SERVER_TIMEOUT);
    //   std::cout << "nfds: " << nfds << "\nfds:" << std::endl;
    // for (size_t i = 0; i < _cur_poll_fds.size(); i++) {
    //   std::cout << _cur_poll_fds[i].fd << ", ";
    // }
    // std::cout  << std::endl;
    // if (nfds == 0) {
    //   std::cout << "Polling arrary size: " << _cur_poll_fds.size() << std::endl;
    //   std::cout << "State arrary size: " << _states.size() << std::endl;
    // }
    // else
    //   std::cout << "nfds: " << nfds << std::endl;
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
        std::list<State>::iterator cur_state = getState(it->fd);

        if (cur_state == _states.end()) {
          std::cerr << "Fd: " << it->fd << std::endl;
          throw std::runtime_error(STATE_NOT_FOUND);
        }
        if (it->revents & POLLNVAL) {
      std::cerr << "Close conn by invald fd in poll: " << cur_state->event_ct << std::endl;
          close_conn(cur_state);
          continue;
        }
        if (it->revents & (POLLHUP | POLLERR)) {
          // Close connection on error (http/1.1 keeps the connection)
          if (it->fd == cur_state->conn_fd) {
      std::cerr << "Close conn by hup/err" << std::endl;
            close_conn(cur_state);
          }
          else if (it->fd == cur_state->file_fd)
            handle_error(*cur_state, UNDEFINED, READ_FILE_FAILURE, *this);
          else if (it->revents & (POLLHUP)) {
            cur_state->stage(cur_state, *it, *this);
          } else
            handle_error(*cur_state, UNDEFINED, READ_CGI_FAILURE, *this);
          continue;
        }
        // std::cout << "Event triggered fd: " << it->fd << std::endl;
        // std::cout << it->revents << std::endl;
        // if (it->revents & POLLIN)
        //   std::cout << "POLLIN" << std::endl;
        // if (it->revents & POLLOUT)
        //   std::cout << "POLLOUT" << std::endl;
        // if (it->revents & POLLERR)
        //   std::cout << "POLL_ERR" << std::endl;
        // if (it->revents & POLLHUP)
        //   std::cout << "POLLHUP" << std::endl;
        cur_state->timeEvent = std::time(NULL);
        cur_state->event_ct++;
        cur_state->stage(cur_state, *it, *this);
      }
    }
  }
}
