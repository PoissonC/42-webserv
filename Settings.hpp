/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 22:05:06 by ychen2            #+#    #+#             */
/*   Updated: 2024/06/20 01:10:01 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <exception>

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

class Settings {
	public:
		Settings(const ServerConfig  & server);
		std::string	getListen() const;
		void		addServer(const ServerConfig & server);
		friend class Server;
	private:
		int							_socket_fd;
		struct sockaddr_in			_addr;
		std::vector<ServerConfig>	_servers;
};
