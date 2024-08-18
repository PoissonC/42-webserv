#include "Server_helper.hpp"

void close_conn(int fd, std::vector< struct pollfd > &poll_fds, std::vector< std::pair< int, t_state > > &states, std::vector< std::pair< int, t_state > >::iterator &cur_state) {
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

void
close_fds(std::vector< int > &fds) {
	for (std::vector< int >::iterator it = fds.begin(); it != fds.end(); it++) {
		close(*it);
	}
}

bool
is_socket(std::vector< int > &socks, int fd) {
	return std::find(socks.begin(), socks.end(), fd) == socks.end() ? false : true;
}

std::vector< std::pair< int, t_state > >::iterator
get_state(std::vector< std::pair< int, t_state > > &states, int fd) {
	for (std::vector< std::pair< int, t_state > >::iterator it = states.begin(); it != states.end(); it++) {
		if (it->first == fd)
			return it;
	}
	return states.end();
}

std::vector< struct pollfd >::iterator
find_it_in_next_pfds(std::vector< struct pollfd > &poll_fds, std::vector< struct pollfd >::iterator &tar) {
	for (std::vector< struct pollfd >::iterator it = poll_fds.begin(); it != poll_fds.end(); it++) {
		if (it->fd == tar->fd)
			return it;
	}
	return poll_fds.end();
}
