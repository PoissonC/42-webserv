/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerPublicMemberFuncs.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:08:17 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/20 19:13:22 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::add_to_poll(int fd) {
  struct pollfd new_pfd;
  bzero(&new_pfd, sizeof(struct pollfd));
  new_pfd.events = POLLIN | POLLHUP | POLLERR;
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
