/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_stages.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:17:26 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/26 15:11:44 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_stages.hpp"
#include "handle_error_response.hpp"
#include "helper.hpp"
#include "pages/pages.hpp"

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
  if (pipe(state.cgi_pipe_w) < 0) {
    handle_error_response(state, 500, "Pipe failed when try to send data to CGI\n", server);
    return;
  }
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
  if (pipe(state.cgi_pipe_r) < 0) {
    handle_error_response(state, 500, "Server pipe failed, can't execute the cgi program.", server);
    return;
  }

  state.req.setEnvCGI(state, server.get_env());
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

  exe_cgi(state, server);
  wait_to_read_cgi(state, server);
}
