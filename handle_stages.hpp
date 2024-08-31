/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_stages.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 17:17:12 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/27 16:41:19 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "MiddleStages.hpp"
#include "Server_helper.hpp"

void handle_read_file(State &state, Server & server);
void handle_save_file(State &state, Server & server);
void handle_cgi(State &state, Server & server);
void handle_delete_file(State & state, Server & server);
bool exe_cgi(State & state, Server & server);
