/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_request.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 18:06:50 by jhurpy            #+#    #+#             */
/*   Updated: 2024/08/24 22:38:53 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ServerConfig.hpp"
# include "Server.hpp"
# include "State.hpp"
# include "handle_stages.hpp"
# include "handle_error_response.hpp"
# include <exception>

static bool compare_method(State & state, LocationConfig & location, Server & server)
{
	e_methods method = state.req.getMethod();
	if (!location.getAllowMethods(method))
	{
		handle_error_response(state, 405, "Request method is forbidden.\n", server);
		return false;
	}
	return true;
}

static LocationConfig & compare_location(State & state, std::map< std::string, LocationConfig > & locationMap)
{
    std::string path = state.req.getUriComponents().path;
		std::cout << "The whole path in uri: " << path << std::endl;
    
    while (!path.empty())
    {
        for (std::map< std::string, LocationConfig >::iterator it = locationMap.begin(); it != locationMap.end(); it++)
        {
            if (it->first == path || it->first == path + "/")
            {
                // Save the remaining part of the original path after truncating the matching part
                state.original_path = state.req.getUriComponents().path.substr(path.length());

                return it->second;
            }
        }
        path = path.substr(0, path.find_last_of('/'));
    }

    // If no match is found, save the full original path
    state.original_path = state.req.getUriComponents().path;
    return locationMap["/"];
}


void handle_request(State & state, Server & server)
{
	server.getServerConfig(state);
	std::map< std::string, LocationConfig > location = state.server.getLocations();

	state.loc = compare_location(state, location);
	if (!compare_method(state, state.loc, server))
		return;
								std::cout << "original_path: " << state.original_path << std::endl;
								std::cout << "root of location: " << state.loc.getRoot() << std::endl;
	if (state.loc.getCgiPass().size() > 0) {
		state.cgi_path = state.loc.getCgiPass();
		handle_cgi(state, server);
		return;
	}
	state.file_path = state.loc.getRoot() + state.original_path;
	if (*state.file_path.rbegin() == '/')
		state.file_path.resize(state.file_path.size() - 1);
						std::cout << "file_path: " << state.file_path << std::endl;
	if (state.req.getMethod() == GET) {
		handle_read_file(state, server);
	}
	else if (state.req.getMethod() == POST) {
		handle_save_file(state, server);
	}
	else if (state.req.getMethod() == DELETE) {
    std::vector<char*> arg;
    arg.push_back(const_cast<char*>(state.file_path.c_str()));
		pid_t pid = fork();
		if (pid < 0)
			throw std::runtime_error("rm fork failed.");
		if (pid == 0)
    	execve("rm", arg.data(), server.get_env());
	}
}
