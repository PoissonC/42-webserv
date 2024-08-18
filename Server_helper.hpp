#include "Server.hpp"

void close_conn(int fd, std::vector< struct pollfd > &poll_fds, std::vector< std::pair< int, t_state > > &states, std::vector< std::pair< int, t_state > >::iterator &cur_state);
void close_fds(std::vector< int > &fds);
bool is_socket(std::vector< int > &socks, int fd);
std::vector< std::pair< int, t_state > >::iterator get_state(std::vector< std::pair< int, t_state > > &states, int fd);
std::vector< struct pollfd >::iterator find_it_in_next_pfds(std::vector< struct pollfd > &poll_fds, std::vector< struct pollfd >::iterator &tar);
