/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 18:32:36 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/08 15:27:11 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MiddleStages.hpp"
#include "constants.hpp"
#include "handle_error.hpp"

void save_file(std::list<State>::iterator &state, const struct pollfd &pfd,
               Server &server) {
  if (!(pfd.revents & POLLOUT))
    return;

  ssize_t wc = write(pfd.fd, state->req.getBody().c_str() + state->bytes_sent,
                     state->req.getBody().size() - state->bytes_sent);

  if (wc < 0) {
    close(pfd.fd);
    server.remove_from_poll(pfd.fd);

    return handle_error(*state, UNDEFINED, SAVE_FILE_FAILURE, server);
  }
  if (wc == (long)state->req.getBody().size() - state->bytes_sent) {
    close(pfd.fd);
    server.remove_from_poll(pfd.fd);
    return wait_to_send_resonpse(*state, server);
  }
  state->bytes_sent += wc;
}