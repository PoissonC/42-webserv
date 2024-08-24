#include "Server.hpp"

void close_fds(std::vector< int > &fds);

void poll_to_out(int fd, Server & server);
void poll_to_in(int fd, Server & server);
void wait_to_read_file(State &state, Server & server);
void wait_to_save_file(State &state, Server & server);
void wait_cgi(State &state, Server & server);
void select_loc_config(State & state, Server & server);