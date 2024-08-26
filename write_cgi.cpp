/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_cgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 23:04:48 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "Server_helper.hpp"
#include "handle_stages.hpp"
#include "handle_error_response.hpp"

void write_cgi(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;
  if (state->cgi_pipe_w[1] != pfd.fd) {
    throw std::runtime_error("Control Flow is Wrong\n");
    return;
  }

  
  ssize_t wc = write(pfd.fd, state->cgi_buff.c_str() + state->bytes_sent,
                    state->cgi_buff.size() - state->bytes_sent);

  if (wc < 0) {
    handle_error_response(*state, 500, "Failed to send data to CGI", server);
    return;
  }

  if (wc == (long)state->cgi_buff.size() - state->bytes_sent) {
    close(state->cgi_pipe_w[1]);
    state->bytes_sent = 0;
    state->cgi_buff = std::string();
    exe_cgi(*state, server);
    wait_to_read_cgi(*state, server);
  } else {
    state->bytes_sent += wc;
  }
}