#include "Server.hpp"

void close_fds(std::vector< int > &fds);

void poll_to_out(int fd, Server & server);
void poll_to_in(int fd, Server & server);
void wait_to_read_file(State &state, Server & server);
void wait_to_save_file(State &state, Server & server);
void wait_to_read_cgi(State &state, Server & server);
void wait_to_write_cgi(State &state, Server & server);
void wait_to_send_resonpse(State &state, Server & server);
void handle_request(State & state, Server & server);