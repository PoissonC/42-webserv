/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/06/20 01:21:36 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

typedef struct s_state {
	// If the size of buffer is too large, we can consider to put this into a external file
	std::string			buffer;
	bool				sent;
	unsigned char *		client_ip;
	// std::ifstream		file;
} t_state;

void	fill_write_buffer(t_state *state, std::string const &response);