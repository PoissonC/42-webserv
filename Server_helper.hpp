#include "Server.hpp"

void close_fds(std::vector< int > &fds);

void poll_to_out(int fd, Server & server);
void poll_to_in(int fd, Server & server);
void wait_to_read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void wait_to_save_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void wait_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);