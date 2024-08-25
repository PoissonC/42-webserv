/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_error_response.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 20:07:38 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/24 21:43:03 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"
#include "Server_helper.hpp"
#include <algorithm>


void handle_error_response(State & state, int status_code, const std::string body, Server & server);