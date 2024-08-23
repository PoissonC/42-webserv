/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhurpy <jhurpy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:29 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 00:40:51 by jhurpy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Server;

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

static void handle_read_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  state->file_fd = open(state->file_path.c_str(), O_RDONLY);
  if (state->file_fd < 0) {
    // TODO: handle error
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
    return;
  }
  wait_to_read_file(state, pfd, server);
}

static void handle_save_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  state->file_fd = open(state->file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (state->file_fd < 0) {
    // TODO: handle error
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
    return;
  }
  wait_to_save_file(state, pfd, server);
}

static void handle_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (pipe(state->cgi_pipe) < 0) {
    // TODO: handle error
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
    return;
  }
  wait_cgi(state, pfd, server);
  pid_t cgi_proc = fork();

  if (cgi_proc < 0)
    throw std::runtime_error("Cgi fork failed");
  if (cgi_proc == 0) {
    if (dup2(state->cgi_pipe[1], STDOUT_FILENO) < 0)
      throw std::runtime_error("Child process dup2 failed");
    close(state->cgi_pipe[0]);
    close(state->cgi_pipe[1]);
    std::vector<char*> arg;
    arg.push_back(const_cast<char*>(state->request_buff.c_str()));
    execve(state->cgi_path.c_str(), arg.data(), server.get_env());
  }
}

void read_request(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLIN))
    return;

  char buf[BUFFER_SIZE];
  ssize_t rc = recv(state->conn_fd, buf, BUFFER_SIZE, MSG_DONTWAIT);

  // < 0 ..> an error occurs, = 0 client closes the connection
  if (rc <= 0) {
    server.close_conn(state->conn_fd, state);
    return;
  }

  state->request_buff += buf;

  if (rc < BUFFER_SIZE) {
    // finish reading, it needs to do something and checks conditions. Below
    // just for tests:
    state->req = Request(buf);
    state->stage = &send_response;
    poll_to_out(state->conn_fd, server);
    return;
    // above needs to be removed


    /*
      TODO: check the config file and decide which case to handle
      Jeremy's job
    */
    // Function will compare the request with the server config
	select_server(*state, server);
    //func(serverConfig);


    server.getServerConfig(*state);




    // if we need to return a file:
    handle_read_file(state, pfd, server);
    // if we need to save a uploaded file:
    handle_save_file(state, pfd, server);
    // if we need to call cgi
    handle_cgi(state, pfd, server);

  }
}
