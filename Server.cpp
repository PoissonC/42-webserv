/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:05:27 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/18 13:40:51 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


bool Server::_constructed = false;
std::string processRequest(std::string request, std::vector< ServerConfig > settings, unsigned char *client_ip);


Server::Server(std::vector< Settings > &settings) : _settings(settings) {
	if (_constructed)
		throw AlreadyConstructed();
	_constructed = true;

	// set socket for all servers, bind and listen.
	{
		for (std::vector< Settings >::iterator it = settings.begin(); it != settings.end(); it++) {
			int new_socket_fd = socket(AF_INET, SOCK_STREAM, 0);// Create the socket without SOCK_NONBLOCK

			if (new_socket_fd < 0) {
				throw std::runtime_error("socket failed");
			}
			it->_socket_fd = new_socket_fd;
			_socks_fd.push_back(new_socket_fd);
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
			{
				struct pollfd new_pfd;
				bzero(&new_pfd, sizeof(struct pollfd));
				new_pfd.events = POLLIN | POLLHUP | POLLERR;
				new_pfd.fd = new_socket_fd;
				_cur_poll_fds.push_back(new_pfd);
			}
		}
	}
}


void Server::run() {
	int nfds;
	std::vector< std::pair< int, t_state > > states;

	_next_poll_fds = _cur_poll_fds;
	while (1) {
		nfds = poll(_cur_poll_fds.data(), _cur_poll_fds.size(), -1);
		if (nfds == -1)
			throw std::runtime_error("poll failed");

		// Process all returned events
		for (std::vector< struct pollfd >::iterator it = _cur_poll_fds.begin(); it != _cur_poll_fds.end(); it++) {
			if (it->revents == 0)
				continue;
			// if (it->revents == POLLIN)
			// Check if the event is for a server socket
			if (is_socket(_socks_fd, it->fd)) {
				int new_sd;
				// Accept new connections
				struct sockaddr_in addr_client;
				socklen_t client_addr_len = sizeof(addr_client);
				while ((new_sd = accept(it->fd, (sockaddr *) &addr_client, &client_addr_len)) != -1) {
					struct pollfd new_pfd;
					bzero(&new_pfd, sizeof(struct pollfd));
					new_pfd.fd = new_sd;
					new_pfd.events = POLLIN | POLLHUP | POLLERR;
					_next_poll_fds.push_back(new_pfd);

					t_state new_conn;
					new_conn.sent = false;
					new_conn.client_ip = (unsigned char *) &addr_client.sin_addr.s_addr;
					states.push_back(std::make_pair(new_sd, new_conn));
					std::cout << "New connection" << std::endl;
				}
			}
			// If the event is not in _socks_fd, handle recv/send
			else {
				char buffer[BUFFER_SIZE];
				std::vector< std::pair< int, t_state > >::iterator cur_state;

				cur_state = get_state(states, it->fd);
				if (cur_state == states.end())
					throw std::runtime_error("State not found");

				// Close connection if any error occurs (http/1.1 keeps the connection)
				if (it->revents & (POLLHUP | POLLERR)) {
					close_conn(it->fd, _next_poll_fds, states, cur_state);
					continue;
				}
				if (it->revents & POLLOUT) {
					// Handle write event
					int wc = 0;
					if (cur_state->second.buffer.size()) {
						wc = send(it->fd, cur_state->second.buffer.c_str(), cur_state->second.buffer.size(), 0);
					}
					if (wc < 0)
						perror("send() failed");
					else {
						// erase the buffer anyway, and keep the connection open
						cur_state->second.buffer.erase(0, wc);
						cur_state->second.sent = false;
						// If the buffer is empty, we wait for the next request
						if (cur_state->second.buffer.empty()) {
							find_it_in_next_pfds(_next_poll_fds, it)->events = POLLIN | POLLHUP | POLLERR;
						}
					}
				}
				else if (it->revents & POLLIN) {
					int	rc;

					rc = recv(it->fd, buffer, sizeof(buffer), 0);
					std::vector< Settings >::iterator targets = _settings.begin();
					for (; targets != _settings.end(); targets++) {
						if (targets->_socket_fd == cur_state->first)
							break;
					}
					if (targets == _settings.end())
						targets = _settings.begin();
					if (rc < 0)
						perror("recv() failed");
					else {
						cur_state->second.buffer.append(buffer, rc);
						// The reading ends.
						if (rc < BUFFER_SIZE) {
							// std::cout  <<"Request: " << cur_state->second.buffer << std::endl;
							if (cur_state->second.buffer.empty()) {
								close_conn(it->fd, _next_poll_fds, states, cur_state);
								continue;
							} else if (cur_state->second.sent == false) {
								cur_state->second.buffer = processRequest(cur_state->second.buffer, targets->_servers, cur_state->second.client_ip);
								cur_state->second.buffer = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 13\n\nHello World!\n";
								std::cout << "Response: " << cur_state->second.buffer << std::endl;
								find_it_in_next_pfds(_next_poll_fds, it)->events = POLLOUT | POLLHUP | POLLERR;
								cur_state->second.sent = true;
							}
						}
					}
				}
			}
		}
		_cur_poll_fds = _next_poll_fds;
	}
}

Server::~Server() {
	close_fds(_socks_fd);
}

// Exceptions

const char *Server::AlreadyConstructed::what() const throw() {
	return "The server instance is already constructed.";
}
