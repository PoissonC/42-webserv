/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_stages.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:17:26 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 21:14:35 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_stages.hpp"
#include "handle_error_response.hpp"
#include "helper.hpp"

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
  state.cgi_buff = state.req.getBody();
  wait_to_write_cgi(state, server);
}

static void check_cgi_extension(State & state) {
  size_t dotPos = state.cgi_path.find_last_of('.');
  if (dotPos == std::string::npos)
    throw std::runtime_error("CGI file doesn't have extension.\n");
  std::string extension = state.cgi_path.substr(dotPos);
  // Convert extension to lowercase
  for (size_t i = 0; i < extension.length(); ++i) {
      extension[i] = std::tolower(extension[i]);
  }
  std::string programName;
  if (extension == ".py")
    programName = "python3";
  else if (extension == ".sh")
    programName = "sh";
  else if (extension == ".php")
    programName = "php-cgi";
  else
    programName = "bash";
  state.original_path = find_cgi_path(programName);
}

void exe_cgi(State & state, Server & server) {
  state.req.setEnvCGI(state.cgi_path, server.get_env());

  pid_t cgi_proc = fork();

  if (cgi_proc < 0)
    handle_error_response(state, 500, "Fail to fork the process to call CGI.", server);
  if (cgi_proc == 0) {
    if (state.cgi_pipe_w[0] != 0) {
      if (dup2(state.cgi_pipe_w[0], STDIN_FILENO) < 0)
        throw std::runtime_error("Child process dup2 failed");
      close(state.cgi_pipe_w[0]);
    }
    if (dup2(state.cgi_pipe_r[1], STDOUT_FILENO) < 0)
      throw std::runtime_error("Child process dup2 failed");
    close(state.cgi_pipe_r[0]);
    close(state.cgi_pipe_r[1]);
    std::vector<char*> file_path;
    // The following function modify state.cgi_path to the path to cgi program
    check_cgi_extension(state);
    file_path.push_back(const_cast<char*>(state.original_path.c_str()));
    file_path.push_back(const_cast<char*>(state.cgi_path.c_str()));
    file_path.push_back(NULL);
    execve(state.original_path.c_str(), file_path.data(), state.req.getEnvCGI());
    throw std::runtime_error("Fail to execute the CGI program.");
  }
  if (state.cgi_pipe_w[0] != 0)
    close(state.cgi_pipe_w[0]);
  close(state.cgi_pipe_r[1]);
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
  exe_cgi(state, server);
  wait_to_read_cgi(state, server);
}
