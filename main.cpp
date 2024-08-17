/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:04:57 by ychen2            #+#    #+#             */
/*   Updated: 2024/05/30 18:11:57 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include "Server.hpp"
#include <iostream>
#include <cerrno>
#include <cstdio>

int main (int ac, char **av) {
	if (ac != 2 && ac != 1) {
		std::cerr << "Usage: " << av[0] << " <config file>(optional)" << std::endl;
		return 1;
	}
	try{
		std::string filename = "config.txt";
		if (ac == 2)
			filename = av[1];
		std::vector<Settings> settings;
		parse(settings, filename);
		Server server(settings);
		server.run();
	}
	catch (std::exception & e){
		if (errno != 0)
			perror(e.what());
		else
			std::cerr << e.what() << std::endl;
	}

	return 0;
}