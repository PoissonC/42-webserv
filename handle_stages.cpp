/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_stages.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:17:26 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 18:55:02 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_stages.hpp"
#include "handle_error_response.hpp"

std::string getMimeType(const std::string& fileName);

static void check_index(State & state) {
  std::vector<std::string> indices = state.loc.getIndex();
  for (std::vector<std::string>::iterator it = indices.begin(); it != indices.end(); it++) {
    std::string tar = state.file_path + "/" + *it;
    state.file_fd = open(tar.c_str(), O_RDONLY);
    state.res.setHeader("Content-Type", getMimeType(tar));
    if (state.file_fd > 0)
      return;
  }
  std::cout << "No file found." << std::endl;
}

void handle_read_file(State & state, Server & server) {
  if (state.file_path.size() <= 2) {
    
    check_index(state);
  }
  else {
    state.file_fd = open(state.file_path.c_str(), O_RDONLY);
    state.res.setHeader("Content-Type", getMimeType(state.file_path));
  }
  if (state.file_fd < 0) {
    // TODO: handle error
    handle_error_response(state, 404, "File not found or can't be opened", server);
    std::cout << "Failed to open file." << std::endl;
    return;
  }
  wait_to_read_file(state, server);
}

void handle_save_file(State & state, Server & server) {
  state.file_fd = open(state.file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (state.file_fd < 0) {
    // TODO: handle error
    handle_error_response(state, 500, "Server can't save the file.", server);
    return;
  }
  wait_to_save_file(state, server);
}

void handle_delete_file(State & state, Server & server) {
  if (remove(state.file_path.c_str()) < 0) {
    handle_error_response(state, 500, "Server can't delete the file.", server);
    return;
  }
  state.res.setBody("File is deleted successfully");
  wait_to_send_resonpse(state, server);
}

static void cgi_post(State & state, Server & server) {
  // add Jeremy's part
  state.cgi_buff = state.req.getBody();
  wait_to_write_cgi(state, server);
}

void exe_cgi(State & state, Server & server) {
  pid_t cgi_proc = fork();

  if (cgi_proc < 0)
    throw std::runtime_error("Cgi fork failed");
  if (cgi_proc == 0) {
    if (state.cgi_pipe_w[0] != 0) {
    if (dup2(state.cgi_pipe_w[0], STDIN_FILENO) < 0)
      throw std::runtime_error("Child process dup2 failed");
    }
    if (dup2(state.cgi_pipe_r[1], STDOUT_FILENO) < 0)
      throw std::runtime_error("Child process dup2 failed");
    close(state.cgi_pipe_r[0]);
    close(state.cgi_pipe_r[1]);
    std::vector<char*> arg;
    arg.push_back(const_cast<char*>(state.request_buff.c_str()));
    execve(state.cgi_path.c_str(), arg.data(), server.get_env());
  }
}

void handle_cgi(State & state, Server & server) {
  if (state.req.getMethod() == POST) {
    cgi_post(state, server);
    return;
  }
  if (pipe(state.cgi_pipe_r) < 0) {
    handle_error_response(state, 500, "Server pipe failed, can't execute the cgi program.", server);
    return;
  }
  wait_to_read_cgi(state, server);
  exe_cgi(state, server);
}
