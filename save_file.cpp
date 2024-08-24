/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/21 20:07:12 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"

void save_file(std::vector<State>::iterator &state, const struct pollfd &pfd, Server & server) {
  if (!(pfd.revents & POLLOUT))
    return;

  ssize_t wc = write(pfd.fd, state->req.getBody().c_str() + state->bytes_sent,
                    state->req.getBody().size() - state->bytes_sent);

  if (wc < 0){
    close(pfd.fd);
    server.remove_from_poll(pfd.fd);
    // TODO: handle error
    state->stage = &send_response;
    server.add_to_poll_out(state->conn_fd);
    state->bytes_sent = 0;
    return;
  }
  if (wc == (long)state->req.getBody().size() - state->bytes_sent) {
    close(pfd.fd);
    server.remove_from_poll(pfd.fd);
    // TODO: generates the response to indicate succsess
    state->stage = &send_response;
    server.add_to_poll_out(state->conn_fd);
    state->bytes_sent = 0;
    return;
  }
  state->bytes_sent += wc;
}