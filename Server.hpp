/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 22:00:02 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/17 20:23:35 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <algorithm>
#include <exception>
#include <sys/epoll.h>
#include <poll.h>
#include "Settings.hpp"
#define BUFFER_SIZE 80
#define BACK_LOG 32
#define MAX_EVENTS 16

class Server {

	public:
		//Constructer, it creates the non-blocking epoll instance, listen to the ip/port from settings
		//	calling epoll_create, socket, setsockopt, bind, listen, epoll_ctl
		Server(std::vector<Settings> & servers);
		~Server();
		//member methods

		//	Start waiting for events
		//		calling epoll_wait, accept, recv, send
		void	run();

		//exceptions
		class AlreadyConstructed: public std::exception {
			public:
				//The server instance is already constructed.
				virtual const char* what() const throw();
		};
		class CreatEpollFail: public std::exception {
			public:
				//epoll_create() failed
				virtual const char* what() const throw();
		};
		class EpollCtlFail: public std::exception {
			public:
				//epoll_ctl() failed
				virtual const char* what() const throw();
		};
		class EpollWaitFail: public std::exception {
			public:
				//epoll_wait() failed
				virtual const char* what() const throw();
		};
		class SocketFail: public std::exception {
			public:
				//socket() failed
				virtual const char* what() const throw();
		};
		class SetSockOptFail: public std::exception {
			public:
				//setsockopt() failed
				virtual const char* what() const throw();
		};
		class BindFail: public std::exception {
			public:
				//bind() failed
				virtual const char* what() const throw();
		};
		class ListenFail: public std::exception {
			public:
				//listen() failed
				virtual const char* what() const throw();
		};
		class AcceptFail: public std::exception {
			public:
				//accept() failed
				virtual const char* what() const throw();
		};

	private:
		static bool					_constructed;
		std::vector<int>			_socks_fd;
		std::vector<struct pollfd>	_cur_poll_fds;
		std::vector<struct pollfd>	_next_poll_fds;
		std::vector<Settings> &	_settings;
};

