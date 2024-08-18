/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:05:27 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/18 16:05:58 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


bool Server::_constructed = false;
std::string processRequest(std::string request, std::vector< ServerConfig > settings, unsigned char *client_ip);

void Server::run_a_server(std::vector< Settings >::iterator & it) {
	int new_socket_fd = socket(AF_INET, SOCK_STREAM, 0);// Create the socket without SOCK_NONBLOCK
	if (new_socket_fd < 0) {
		throw std::runtime_error("socket failed");
	}

	it->_socket_fd = new_socket_fd;
	_socks_fd.push_back(new_socket_fd);

	// To allow address receive multiple connections at the same time
	{
		int on = 1;
		if (setsockopt(new_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0) {
			close_fds(_socks_fd);
			throw std::runtime_error("setsockopt failed");
		}
	}

	if (bind(new_socket_fd, (struct sockaddr *) &(it->_addr), sizeof(it->_addr)) < 0) {
		close_fds(_socks_fd);
		throw std::runtime_error("bind failed");
	}

	if (listen(new_socket_fd, BACK_LOG) < 0) {
		close_fds(_socks_fd);
		throw std::runtime_error("listen failed");
	}

	// Setting up sockets for poll
	add_to_poll(_cur_poll_fds, new_socket_fd);
}


Server::Server(std::vector< Settings > &settings) : _settings(settings) {
	if (_constructed)
		throw std::runtime_error("The server instance already exists");
	_constructed = true;

	// set socket for all servers, bind and listen.
	for (std::vector< Settings >::iterator it = _settings.begin(); it != _settings.end(); it++)
		run_a_server(it);
}


void Server::run() {
	_next_poll_fds = _cur_poll_fds;
	while (1) {
		int nfds = poll(_cur_poll_fds.data(), _cur_poll_fds.size(), -1);
		if (nfds == -1)
			throw std::runtime_error("poll failed");

		// Process all returned events
		for (std::vector< struct pollfd >::iterator it = _cur_poll_fds.begin(); it != _cur_poll_fds.end(); it++) {
			if (it->revents == 0)
				continue;

			// Check if the event is for a server socket
			if (is_socket(_socks_fd, it->fd)) 
				new_conns(_states, _next_poll_fds, it->fd);

			// If the event is not in _socks_fd, handle 5 stages
			else {
				char buffer[BUFFER_SIZE];
				std::vector<t_state>::iterator cur_state = get_state(_states, it->fd);

				if (cur_state == _states.end())
					throw std::runtime_error("State not found");

				// Close connection if any error occurs (http/1.1 keeps the connection)
				if (it->revents & (POLLHUP | POLLERR)) {
					close_conn(it->fd, _next_poll_fds, _states, cur_state);
					continue;
				}

				switch (cur_state->stage) {
				case NEW_CONN:
					new_conn_stage(*cur_state, *it);
					break;
				case READ_REQUEST:
					read_request(*cur_state, *it);
					break;
				case SEND_RESPONSE:
					send_response(*cur_state, *it);
					break;
				case READ_FILE:
					read_file(*cur_state, *it);
					break;
				case FORK_CGI:
					fork_cgi(*cur_state, *it);
					break;
				case READ_CGI:
					read_cgi(*cur_state, *it);
					break;
				}
			}
		}
		_cur_poll_fds = _next_poll_fds;
	}
}

void Server::new_conn_stage(t_state & state, const struct pollfd & pfd) {
	if (pfd.revents & POLLIN) {
		state.stage = READ_REQUEST;
		read_request(state, pfd);
	}
}

void Server::read_request(t_state & state, const struct pollfd & pfd) {
	if (pfd.revents & POLLIN) {
		
		read_request(state, pfd);
	}
}

Server::~Server() {
	close_fds(_socks_fd);
}

