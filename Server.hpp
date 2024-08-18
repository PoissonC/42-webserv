/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 22:00:02 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/18 13:17:51 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Settings.hpp"
#include <algorithm>
#include <exception>
#include <poll.h>
#include <vector>
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

		//exceptions
		class AlreadyConstructed: public std::exception {
			public:
				//The server instance is already constructed.
				virtual const char* what() const throw();
		};

private:
	static bool _constructed;
	std::vector< int > _socks_fd;
	std::vector< struct pollfd > _cur_poll_fds;
	std::vector< struct pollfd > _next_poll_fds;
	std::vector< Settings > &_settings;
};
