/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 22:00:02 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/18 16:04:32 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Settings.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <utility>
#include <exception>
#include <poll.h>
#include <vector>
#include "State.hpp"
#include "Server_helper.hpp"
#ifdef __APPLE__
#include <fcntl.h>
#endif

#define BUFFER_SIZE 80
#define BACK_LOG 32
#define MAX_EVENTS 16

class Server {

public:
	//Constructer, it creates the non-blocking socket connection, listen to the ip/port from settings
	//	calling socket, setsockopt, bind, listen
	//  put socket fds into pfs
	Server(std::vector<Settings> & servers);
	~Server();
	//member methods

	//	Start waiting for events
	//		calling poll, accept, recv, send
	void	run();

private:
	static bool _constructed;
	std::vector< int > _socks_fd;
	std::vector< struct pollfd > _cur_poll_fds;
	std::vector< struct pollfd > _next_poll_fds;
	std::vector< Settings > &_settings;
	std::vector<t_state> _states;

	// Stages would block
	void	read_request(t_state &, const struct pollfd & pfd);		// open the requested file in the end
	void	send_response(t_state &, const struct pollfd & pfd);
	void	read_file(t_state &, const struct pollfd & pfd);
	void	fork_cgi(t_state &, const struct pollfd & pfd);
	void	read_cgi(t_state &, const struct pollfd & pfd);

	// funcs
	void	new_conn_stage(t_state &, const struct pollfd & pfd);
	void	run_a_server(std::vector< Settings >::iterator &); // socket, setsockopt, bind, listen
	
};
