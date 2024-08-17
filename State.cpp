/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   State.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:25:03 by yu                #+#    #+#             */
/*   Updated: 2024/05/11 17:00:39 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "State.hpp"

void	fill_write_buffer(t_state *state, std::string const &response) {
    for (std::string::const_iterator it = response.begin(); it != response.end(); it++) {
        state->buffer += (*it);
    }
}