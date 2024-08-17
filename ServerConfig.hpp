/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 10:44:59 by yu                #+#    #+#             */
/*   Updated: 2024/05/30 16:56:14 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>
#include "LocationConfig.hpp"



class ServerConfig {
	public:
		ServerConfig();
		// getters
		std::vector<std::string>				getServerNames() const;
		std::string								getListen() const;
		std::map<int, std::string>				getErrorPages() const;
		std::map<std::string, LocationConfig>	getLocations() const;

		// setters
		void									addServerName(const std::vector<std::string> &tokens, size_t &pos);
		void									setListen(const std::vector<std::string> &tokens, size_t &pos);
		void									addErrorPage(const std::vector<std::string> &tokens, size_t &pos);
		void									addLocation(const std::string & path, const LocationConfig &location);
		
		// methods
		bool									hasRootLocation() const;	
	private:
		std::vector<std::string>				_server_name;
		std::string								_listen;
		std::map<std::string, LocationConfig>	_locations;
		std::map<int, std::string>				_error_pages;
};

ServerConfig parseServer(const std::vector<std::string> &tokens, size_t &pos);
