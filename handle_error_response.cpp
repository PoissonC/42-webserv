/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_error_response.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 21:19:18 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/27 16:49:15 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_error_response.hpp"
std::string getMimeType(const std::string& fileName);

void handle_error_response(State & state, int status_code, const std::string body, Server & server) {
  server.remove_from_poll(state.cgi_pipe_r[0]);
  server.remove_from_poll(state.cgi_pipe_w[1]);
  server.remove_from_poll(state.file_fd);
  state.res.setStatusCode(status_code);
  state.res.setBody(body);
  state.bytes_sent = 0;
	std::map< int, std::string > error_pages = state.server.getErrorPages();
  std::map< int, std::string >::iterator it = error_pages.find(status_code);
  // If no default page set
  if (it == error_pages.end()) {
    wait_to_send_resonpse(state, server);
    return;
  }
  state.file_buff = std::string();
  state.file_fd = open(it->second.c_str(), O_RDONLY);
  // Open error page failed
  if (state.file_fd < 0) {
    wait_to_send_resonpse(state, server);
    return;
  }
  state.res.setHeader("Content-Type", getMimeType(it->second.c_str()));
  wait_to_read_file(state, server);
}
