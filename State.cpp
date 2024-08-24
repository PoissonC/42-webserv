/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/24 17:13:41 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"

State::State(int fd, unsigned char * client_addr, int socket): bytes_sent(0), req(std::string()), client_ip(client_addr), sock_fd(socket) , conn_fd(fd), stage(&read_request) {
  std::cout << "New connection fd: " << fd << std::endl;
}
