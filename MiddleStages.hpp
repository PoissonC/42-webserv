/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MiddleStages.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/08/26 14:35:40 by ychen2           ###   ########.fr       */
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

