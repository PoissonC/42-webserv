/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_stages.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:17:26 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 19:58:08 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_stages.hpp"
#include "pages/pages.hpp"

std::string getMimeType(const std::string &fileName);

void handle_read_file(State &state, Server &server) {
  struct stat path_stat;
  stat(state.file_path.c_str(), &path_stat);

  if (S_ISDIR(path_stat.st_mode)) {
    const std::vector<std::string> &indexFiles = state.loc.getIndex();
    bool isIndexFileFound = false;

    for (std::vector<std::string>::const_iterator it = indexFiles.begin();
         it != indexFiles.end(); ++it) {
      std::string potentialIndexPath = state.file_path + "/" + *it;
      if (access(potentialIndexPath.c_str(), F_OK) != -1) {
        state.file_path = potentialIndexPath;
        state.file_fd = open(state.file_path.c_str(), O_RDONLY);
        state.res.setHeader("Content-Type", getMimeType(state.file_path));
        isIndexFileFound = true;
        break;
      }
    }

    if (!isIndexFileFound) {
      state.res.setHeader("Content-Type", "text/html");
      if (state.loc.getAutoindex()) {
        state.res.setBody(getDirectoryPage(state));
        state.response_buff = state.res.generateResponseString();
        state.stage = &send_response;
        poll_to_out(state.conn_fd, server);
        return;
      } else {
        state.res.setBody(getIndexPage(state));
        state.response_buff = state.res.generateResponseString();
        state.stage = &send_response;
        poll_to_out(state.conn_fd, server);
        return;
      }
    }
  } else {
    state.file_fd = open(state.file_path.c_str(), O_RDONLY);
    state.res.setHeader("Content-Type", getMimeType(state.file_path));
  }

  if (state.file_fd < 0) {
    // TODO handle error
    std::cout << "Failed to open file." << std::endl;
    state.stage = &send_response;
    poll_to_out(state.conn_fd, server);
    return;
  }

  wait_to_read_file(state, server);
}

void handle_save_file(State &state, Server &server) {
  state.file_fd =
      open(state.file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (state.file_fd < 0) {
    // TODO: handle error
    state.stage = &send_response;
    poll_to_out(state.conn_fd, server);
    return;
  }
  wait_to_save_file(state, server);
}

void handle_cgi(State &state, Server &server) {
  if (pipe(state.cgi_pipe) < 0) {
    // TODO: handle error
    state.stage = &send_response;
    poll_to_out(state.conn_fd, server);
    return;
  }
  wait_cgi(state, server);
  pid_t cgi_proc = fork();

  if (cgi_proc < 0)
    throw std::runtime_error("Cgi fork failed");
  if (cgi_proc == 0) {
    if (dup2(state.cgi_pipe[1], STDOUT_FILENO) < 0)
      throw std::runtime_error("Child process dup2 failed");
    close(state.cgi_pipe[0]);
    close(state.cgi_pipe[1]);
    std::vector<char *> arg;
    arg.push_back(const_cast<char *>(state.request_buff.c_str()));
    execve(state.cgi_path.c_str(), arg.data(), server.get_env());
  }
}
