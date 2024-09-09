/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_stages.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:17:26 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/09 23:25:58 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_stages.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
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
      state.res.setHeader("Set-Cookie", state.req.createCookie());
      if (state.loc.getAutoindex()) {
        state.res.setBody(getDirectoryPage(state));
        state.response_buff = state.res.generateResponseString();
        state.stage = &send_response;

        return poll_to_out(state.conn_fd, server);
      } else {
        state.res.setBody(getIndexPage(state));
        state.response_buff = state.res.generateResponseString();
        state.stage = &send_response;

        return poll_to_out(state.conn_fd, server);
      }
    }
  } else {
    state.file_fd = open(state.file_path.c_str(), O_RDONLY);
    state.res.setHeader("Content-Type", getMimeType(state.file_path));
  }

  if (state.file_fd < 0)
    return handle_error(state, UNDEFINED, READ_FILE_FAILURE, server);

  wait_to_read_file(state, server);
}

void handle_save_file(State &state, Server &server) {
  state.file_fd =
      open(state.file_path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);
  if (state.file_fd < 0)
    return handle_error(state, UNDEFINED, SAVE_FILE_FAILURE, server);
  state.res.setStatusCode(CREATED);
  wait_to_save_file(state, server);
}

void handle_delete_file(State &state, Server &server) {
  if (std::remove(state.file_path.c_str()) < 0)
    return handle_error(state, UNDEFINED, DELETE_FILE_FAILURE, server);
  if (access(state.file_path.c_str(), F_OK) != 0)
    state.res.setStatusCode(NO_CONTENT);

  wait_to_send_resonpse(state, server);
}

static void cgi_post(State &state, Server &server) {
  if (pipe(state.cgi_pipe_w) < 0)
    return handle_error(state, UNDEFINED, CGI_DATA_SEND_FAILURE, server);

  state.cgi_buff = state.req.getBody();
  wait_to_write_cgi(state, server);
}

static void check_cgi_extension(State &state) {
  size_t dotPos = state.cgi_path.find_last_of('.');
  if (dotPos == std::string::npos)
    throw std::runtime_error(CGI_EXECUTION_FAILURE);
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
  else {
    programName = state.cgi_path;
    state.original_path = programName;
    return;
  }
  state.original_path = find_cgi_path(programName);
}

bool exe_cgi(State &state, Server &server) {
  if (access(state.cgi_path.c_str(), R_OK)) {
    if (access(state.cgi_path.c_str(), F_OK))
      handle_error(state, NOT_FOUND, std::string(), server);
    else
      handle_error(state, FORBIDDEN, std::string(), server);
    return false;
  }
  if (pipe(state.cgi_pipe_r) < 0) {
    handle_error(state, UNDEFINED, CGI_EXECUTION_FAILURE, server);
    return false;
  }

  state.req.setEnvCGI(state, server.get_env());
  state.cgiPID = fork();

  if (state.cgiPID < 0)
    handle_error(state, UNDEFINED, CGI_EXECUTION_FAILURE, server);
  if (state.cgiPID == 0) {
    if (state.cgi_pipe_w[0] != 0) {
      if (dup2(state.cgi_pipe_w[0], STDIN_FILENO) < 0)
        throw std::runtime_error(CGI_EXECUTION_FAILURE);
      close(state.cgi_pipe_w[0]);
    }
    if (dup2(state.cgi_pipe_r[1], STDOUT_FILENO) < 0)
      throw std::runtime_error(CGI_EXECUTION_FAILURE);
    close(state.cgi_pipe_r[0]);
    close(state.cgi_pipe_r[1]);
    std::vector<char *> file_path;
    check_cgi_extension(state);
    file_path.push_back(const_cast<char *>(state.original_path.c_str()));
    file_path.push_back(const_cast<char *>(state.cgi_path.c_str()));
    file_path.push_back(NULL);
    execve(state.original_path.c_str(), file_path.data(),
           state.req.getEnvCGI().data());
    throw std::runtime_error(CGI_EXECUTION_FAILURE);
  }
  state.timeCGI = std::time(NULL);
  state.isCGIrunning = true;
  if (state.cgi_pipe_w[0] != 0) {
    close(state.cgi_pipe_w[0]);
    state.cgi_pipe_w[0] = 0;
  }
  close(state.cgi_pipe_r[1]);
  state.cgi_pipe_r[1] = 0;
  return true;
}

void handle_cgi(State &state, Server &server) {
  if (state.req.getMethod() == POST) {
    cgi_post(state, server);
    return;
  }

  if (!exe_cgi(state, server))
    return;
  wait_to_read_cgi(state, server);
}
