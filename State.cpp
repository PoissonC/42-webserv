/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/21 16:51:53 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"

State::State(int fd, unsigned char * client_addr): req(std::string()), stage(&read_request), bytes_sent(0), client_ip(client_addr), conn_fd(fd) {
  std::cout << "New connection fd: " << fd << std::endl;
}
