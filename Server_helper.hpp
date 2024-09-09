/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_helper.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 17:16:52 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/09 17:53:11 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"

void close_fds(std::vector< int > &fds);

void poll_to_out(int fd, Server & server);
void poll_to_in(int fd, Server & server);
void wait_to_read_file(State &state, Server & server);
void wait_to_save_file(State &state, Server & server);
void wait_to_read_cgi(State &state, Server & server);
void wait_to_write_cgi(State &state, Server & server);
void wait_to_send_resonpse(State &state, Server & server);
void handle_request(State & state, Server & server);
LocationConfig &
compare_location(State &state,
                 std::map<std::string, LocationConfig> &locationMap);