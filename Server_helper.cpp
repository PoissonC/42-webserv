#include "Server_helper.hpp"
void add_to_poll(std::vector< struct pollfd > & pfds, int fd) {
	struct pollfd new_pfd;
	bzero(&new_pfd, sizeof(struct pollfd));
	new_pfd.events = POLLIN | POLLHUP | POLLERR;
	new_pfd.fd = fd;
	pfds.push_back(new_pfd);
}

void new_conns(std::vector<t_state> & states, std::vector< struct pollfd > & poll_fds, int sock_fd) {
	int new_sd;
	// Accept new connections
	struct sockaddr_in addr_client;
	socklen_t client_addr_len = sizeof(addr_client);
	while ((new_sd = accept(sock_fd, (sockaddr *) &addr_client, &client_addr_len)) != -1) {
		struct pollfd new_pfd;
		bzero(&new_pfd, sizeof(struct pollfd));
		new_pfd.fd = new_sd;
		new_pfd.events = POLLIN | POLLHUP | POLLERR;
		poll_fds.push_back(new_pfd);

		t_state new_conn;
		// not finished
		bzero(&new_conn, sizeof(t_state));
		new_conn.stage = NEW_CONN;
		new_conn.conn_fd = new_sd;
		new_conn.client_ip = (unsigned char *) &addr_client.sin_addr.s_addr;
		states.push_back(new_conn);
		std::cout << "New connection fd: " << new_conn.conn_fd << std::endl;
	}
}

void close_conn(int fd, std::vector< struct pollfd > &poll_fds, std::vector<t_state> &states, std::vector<t_state>::iterator &cur_state) {
	close(fd);
	for (std::vector< struct pollfd >::iterator it = poll_fds.begin(); it != poll_fds.end(); it++) {
		if (it->fd == fd) {
			poll_fds.erase(it);
			break;
		}
	}
	// This should destroy the cur_state element (free resources, ChatGPT says so)
	states.erase(cur_state);
	std::cout << "connection ends" << std::endl;
}

void close_fds(std::vector< int > &fds) {
	for (std::vector< int >::iterator it = fds.begin(); it != fds.end(); it++) {
		close(*it);
	}
}

bool is_socket(std::vector< int > &socks, int fd) {
	return std::find(socks.begin(), socks.end(), fd) == socks.end() ? false : true;
}

std::vector<t_state>::iterator
get_state(std::vector<t_state> & states, int fd) {
	for (std::vector<t_state>::iterator it = states.begin(); it != states.end(); it++) {
		if (it->conn_fd == fd)
			return it;
	}
	return states.end();
}

std::vector< struct pollfd >::iterator
find_it_in_pfds(std::vector< struct pollfd > &poll_fds, int fd) {
	for (std::vector< struct pollfd >::iterator it = poll_fds.begin(); it != poll_fds.end(); it++) {
		if (it->fd == fd)
			return it;
	}
	return poll_fds.end();
}
