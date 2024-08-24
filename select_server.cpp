/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhurpy <jhurpy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 18:06:50 by jhurpy            #+#    #+#             */
/*   Updated: 2024/08/24 16:02:23 by jhurpy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ServerConfig.hpp"
# include "Server.hpp"
# include "State.hpp"

static bool compare_method(State & state, LocationConfig * location)
{
	e_methods method = state.req.getMethod();
	if (!location->getAllowMethods(method))
	{
		state.res.setStatusCode(405);
		return false;
	}
	return true;
}

static LocationConfig * compare_location(State & state, std::map< std::string, LocationConfig > & locationMap)
{
	std::string path = state.req.getUriComponents().path;
	LocationConfig *location = NULL;
	while (!path.empty())
	{
		for (std::map< std::string, LocationConfig >::iterator it = locationMap.begin(); it != locationMap.end(); it++)
		{
			if (it->first == path || it->first == path + "/")
			{
				location = &it->second;
				return location;
			}
		}
		path = path.substr(0, path.find_last_of('/'));

	}
	if (locationMap.find("/") != locationMap.end())
		location = &locationMap["/"];
	return location;
}

void select_server(State & state, Server & server)
{
	ServerConfig serverConf = server.getServerConfig(state);
	std::map< std::string, LocationConfig > location = serverConf.getLocations();

	LocationConfig * locationConfig = compare_location(state, location);
	if (locationConfig == NULL)
	{
		state.res.setStatusCode(404);
		return;
	}
	if (!compare_method(state, locationConfig))
		return;

	state.cgi_path = locationConfig->getCgiPass();
	state.file_path = locationConfig->getRoot();
}
