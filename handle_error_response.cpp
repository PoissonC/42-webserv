/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_error_response.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 21:19:18 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 22:34:33 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "handle_error_response.hpp"
std::string getMimeType(const std::string& fileName);

void handle_error_response(State & state, int status_code, const std::string body, Server & server) {
  state.res.setStatusCode(status_code);
  state.res.setBody(body);
	std::map< int, std::string > error_pages = state.server.getErrorPages();
  std::map< int, std::string >::iterator it = error_pages.find(status_code);
  // If no default page set
  if (it == error_pages.end()) {
    state.response_buff = state.res.generateResponseString();
    poll_to_out(state.conn_fd, server);
    state.stage = &send_response;
    return;
  }
  state.file_buff = std::string();
  state.file_fd = open(it->second.c_str(), O_RDONLY);
  // Open error page failed
  if (state.file_fd < 0) {
    poll_to_out(state.conn_fd, server);
    state.stage = &send_response;
    return;
  }
  state.res.setHeader("Content-Type", getMimeType(state.file_path));
  wait_to_read_file(state, server);
}
