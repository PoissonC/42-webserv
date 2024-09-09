/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 16:04:57 by ychen2            #+#    #+#             */
/*   Updated: 2024/09/09 23:26:28 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "parser.hpp"
#include <cerrno>
#include <cstdio>
#include <iostream>

int main(int ac, char **av, char **env) {
	if (ac != 2 && ac != 1) {
		std::cerr << "Usage: " << av[0] << " <config file>(optional)" << std::endl;
		return 1;
	}
	try {
		std::string filename = "config.txt";
		if (ac == 2)
			filename = av[1];
		std::vector< Settings > settings;
		parse(settings, filename);
		Server server(settings, env);
		server.run();
	} catch (std::exception &e) {
		if (errno != 0)
			perror(e.what());
		else
			std::cerr << e.what() << std::endl;
		return(1);
	}
	return 0;
}