/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhurpy <jhurpy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 18:06:50 by jhurpy            #+#    #+#             */
/*   Updated: 2024/08/23 23:28:36 by jhurpy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ServerConfig.hpp"
# include "Server.hpp"
# include "State.hpp"

//static ServerConfig * compare_server_name(State & state)
//{
//	// compare server name with the host
//	// initialize the server config with the first server because it is the default server
//	ServerConfig *server = state.server_config[0]; // TODO define the way to access this data
//	for (std::vector<ServerConfig>::iterator it = state.server_config.begin(); it != state.server_config.end(); it++)
//	{
//		if (it->getServerNames().empty())
//			server = *it;
//		else
//		{
//			for (std::vector<std::string>::iterator it2 = it->getServerNames().begin(); it2 != it->getServerNames().end(); it2++)
//			{
//				if (*it2 == state.req.getHeaders().find("Host")->second)
//				{
//					server = *it;
//					break;
//				}
//			}
//		}
//	}
//	return server;

//}

static void compare_method(State & state, LocationConfig * location)
{
	// compare method with the request method
	e_methods method = state.req.getMethod();
	if (!location->getAllowMethods(method))
	{
		state.res.setStatusCode(405);
	}
}

static LocationConfig * compare_location(State & state, ServerConfig server)
{
	// compare location with the path
	std::string path = state.req.getUriComponents().path;
	std::map<std::string, LocationConfig> locations = server.getLocations();
	LocationConfig *location = NULL;
	for (std::map<std::string, LocationConfig>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		if (path.find(it->first) == 0)
		{
			location = &it->second;
			break;
		}
	}
	if (location == NULL)
	{
		state.res.setStatusCode(404);
		return NULL;
	}
	else
	{
		compare_method(state, location);
		if (state.res.getStatusCode() != 200)
			return NULL;
	}
	return location;
}

void select_server(State & state, Server & server)
{
	//ServerConfig *server = compare_server_name(state);
	ServerConfig serverConf = server.getServerConfig(state);
	if (serverConf.getServerNames().empty())
		state.res.setStatusCode(404); // confirm if this is relevant regarding the config file
	if (state.res.getStatusCode() == 200)
		LocationConfig *location = compare_location(state, serverConf);
	if (state.res.getStatusCode() == 200)
	{
		// handle if path is for file_path or cgi_path
		// TODO
	}
}
