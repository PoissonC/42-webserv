/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_error.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 20:07:38 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/26 17:16:30 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server_helper.hpp"
#include "State.hpp"
#include <algorithm>

void handle_error(State &state, int statusCode, const std::string &context,
                  Server &server);