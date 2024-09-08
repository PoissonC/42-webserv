/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/08 20:30:32 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "constants.hpp"
#include "handle_error.hpp"

void save_file(std::list<State>::iterator &state, const struct pollfd &pfd,
               Server &server) {
  if (!(pfd.revents & POLLOUT))
    return;

  ssize_t wc = write(state->file_fd, state->req.getBody().c_str() + state->bytes_sent,
                     state->req.getBody().size() - state->bytes_sent);

  if (wc < 0) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    state->file_fd = 0;

    return handle_error(*state, UNDEFINED, SAVE_FILE_FAILURE, server);
  }
  if (wc == (long)state->req.getBody().size() - state->bytes_sent) {
    close(state->file_fd);
    server.remove_from_poll(state->file_fd);
    state->file_fd = 0;
    return wait_to_send_resonpse(*state, server);
  }
  state->bytes_sent += wc;
}