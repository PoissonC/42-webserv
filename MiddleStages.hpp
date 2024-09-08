/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MiddleStages.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:22:54 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/08 15:26:13 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

class Server;
class State;
#include "State.hpp"
#include "Server.hpp"


void read_request(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server);
void send_response(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server);
void read_file(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server);
void save_file(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server);
void read_cgi(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server);
void write_cgi(std::list<State>::iterator &state, const struct pollfd &pfd, Server & server);

