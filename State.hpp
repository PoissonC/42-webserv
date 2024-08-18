/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:14:22 by yu                #+#    #+#             */
/*   Updated: 2024/08/18 15:52:22 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

typedef enum e_stage {
	NEW_CONN,
	READ_REQUEST,
	SEND_RESPONSE,
	READ_FILE,
	FORK_CGI,
	READ_CGI
} t_stage;

typedef struct s_state {
	std::string request_buff;
	std::string file_buff;

	unsigned char *client_ip;

	int	conn_fd, file_fd;

	t_stage		stage;
} t_state;
