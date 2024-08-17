/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 21:08:04 by ychen2            #+#    #+#             */
/*   Updated: 2024/05/30 18:55:24 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Settings.hpp"
#include <cstdlib>
#include <arpa/inet.h>

Settings::Settings(const ServerConfig & server) {
	std::string listen = server.getListen();
	size_t end = listen.find_first_of(":", 0);
	if (end == std::string::npos) {
		throw std::runtime_error("Invalid listen string: " + listen);
	}

	std::string ip_str = listen.substr(0, end);
	std::string port_str = listen.substr(end + 1);

	struct in_addr ip_addr;
	if (inet_pton(AF_INET, ip_str.c_str(), &ip_addr) != 1) {
		throw std::runtime_error("Invalid IP address: " + ip_str);
	}

	int port = std::atoi(port_str.c_str());
	if (port < 1 || port > 65535) {
		throw std::runtime_error("Invalid port number: " + port_str);
	}

	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr = ip_addr;
	_addr.sin_port = htons(port);

	_servers.push_back(server);
}

std::string	Settings::getListen() const {
	return _servers[0].getListen();
}

void	Settings::addServer(const ServerConfig & server) {
	_servers.push_back(server);
}
