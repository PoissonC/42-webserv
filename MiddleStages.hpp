/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MiddleStages.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhurpy <jhurpy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:22:54 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 00:43:27 by jhurpy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Server;
class State;
#include "State.hpp"
#include "Server.hpp"


void read_request(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void send_response(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void save_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void read_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);

void select_server(State & state, Server & server);
