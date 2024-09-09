/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 18:06:50 by jhurpy            #+#    #+#             */
/*   Updated: 2024/09/09 17:53:24 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "State.hpp"
#include "constants.hpp"
#include "handle_error.hpp"
#include "handle_stages.hpp"
#include "utils/utils.hpp"

static bool compare_method(State &state, LocationConfig &location,
                           Server &server) {
  e_methods method = state.req.getMethod();
  if (!location.getAllowMethods(method)) {
    handle_error(state, UNDEFINED, FORBIDDEN_REQUEST_METHOD, server);
    return false;
  }
  return true;
}

LocationConfig &
compare_location(State &state,
                 std::map<std::string, LocationConfig> &locationMap) {
  std::string path = state.req.getUriComponents().path;
  // std::cout << "The whole path in uri: " << path << std::endl;

  while (!path.empty()) {
    for (std::map<std::string, LocationConfig>::iterator it =
             locationMap.begin();
         it != locationMap.end(); it++) {
      if (it->first == path || it->first == path + "/") {
        // Save the remaining part of the original path after truncating the
        // matching part
        state.original_path =
            state.req.getUriComponents().path.substr(path.length());

        return it->second;
      }
    }
    path = path.substr(0, path.find_last_of('/'));
  }

  // If no match is found, save the full original path
  state.original_path = state.req.getUriComponents().path;
  return locationMap["/"];
}

void handle_request(State &state, Server &server) {
  if (!compare_method(state, state.loc, server))
    return;
  // std::cout << "original_path: " << state.original_path << std::endl;
  // std::cout << "root of location: " << state.loc.getRoot() << std::endl;
  std::pair<int, std::string> rdr = state.loc.getRedir();
  if (rdr.first != 0) {
    state.res.setStatusCode(rdr.first);
    state.res.setHeader("Location", rdr.second);
    wait_to_send_resonpse(state, server);
    return ;
  }
  if (state.loc.getCgiPass().size() > 0) {
    state.cgi_path =
        state.loc.getRoot() + state.loc.getCgiPass() + state.original_path;
    // std::cout << "Full CGI path: " << state.cgi_path << std::endl;

    struct stat path_stat;
    stat(state.cgi_path.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode)) {
      state.file_path = state.cgi_path;
      handle_read_file(state, server);
      return;
    }
    handle_cgi(state, server);
    return;
  }
  state.file_path = state.loc.getRoot() + state.original_path;
  if (*state.file_path.rbegin() == '/')
    state.file_path.resize(state.file_path.size() - 1);
  // std::cout << "file_path: " << state.file_path << std::endl;
  if (state.req.getMethod() == GET) {
    handle_read_file(state, server);
  } else if (state.req.getMethod() == POST) {
    if (state.file_path.find("../") != std::string::npos)
      return handle_error(state, BAD_REQUEST, READ_FILE_FAILURE, server);
    handle_save_file(state, server);
  } else if (state.req.getMethod() == DELETE) {
    if (state.file_path.find("../") != std::string::npos)
      return handle_error(state, BAD_REQUEST, READ_FILE_FAILURE, server);

    handle_delete_file(state, server);
  }
}
