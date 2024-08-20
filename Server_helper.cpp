#include "Server_helper.hpp"


void close_fds(std::vector<int> &fds) {
  for (std::vector<int>::iterator it = fds.begin(); it != fds.end(); it++) {
    close(*it);
  }
}



