#include "Server.hpp"

void
	add_to_poll(std::vector< struct pollfd > & pfds, int fd);

void
	new_conns(std::vector<t_state> & states, std::vector< struct pollfd > & poll_fds, int sock_fd);

void
	close_conn(int fd, std::vector< struct pollfd > &poll_fds, std::vector<t_state> &states, std::vector<t_state>::iterator &cur_state);

void
	close_fds(std::vector< int > &fds);

bool
	is_socket(std::vector< int > &socks, int fd);

std::vector<t_state>::iterator
	get_state(std::vector<t_state> &states, int fd);

std::vector< struct pollfd >::iterator
	find_it_in_next_pfds(std::vector< struct pollfd > &poll_fds, std::vector< struct pollfd >::iterator &tar);
