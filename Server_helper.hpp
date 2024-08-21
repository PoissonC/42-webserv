#include "Server.hpp"

void close_fds(std::vector< int > &fds);

void wait_to_respond(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void wait_to_read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);
void wait_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server);