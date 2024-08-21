#include "Server_helper.hpp"


void close_fds(std::vector<int> &fds) {
  for (std::vector<int>::iterator it = fds.begin(); it != fds.end(); it++) {
    close(*it);
  }
}

void poll_to_out(int fd, Server & server) {
  std::vector<struct pollfd>::iterator next_pfd =
      server.find_it_in_nxt(fd);
  // if the fd is not in the poll array
  if (next_pfd == server.getNextPfdsEnd()) {
    server.add_to_poll_out(fd);
    return;
  }
  // else
  next_pfd->events = POLLOUT | POLLHUP | POLLERR;
}

void poll_to_in(int fd, Server & server) {
  std::vector<struct pollfd>::iterator next_pfd =
      server.find_it_in_nxt(fd);
  // if the fd is not in the poll array
  if (next_pfd == server.getNextPfdsEnd()) {
    server.add_to_poll_in(fd);
    return;
  }
  // else
  next_pfd->events = POLLIN | POLLHUP | POLLERR;
}

void wait_to_save_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  state->stage = &save_file;
  server.remove_from_poll(state->conn_fd);
  server.add_to_poll_out(state->file_fd);
}

void wait_to_read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  state->stage = &read_file;
  server.remove_from_poll(state->conn_fd);
  server.add_to_poll_in(state->file_fd);
}

void wait_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  state->stage = &read_cgi;
  server.remove_from_poll(state->conn_fd);
  server.add_to_poll_in(state->cgi_pipe[0]);
}

