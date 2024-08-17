/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 10:44:50 by yu                #+#    #+#             */
/*   Updated: 2024/05/20 02:01:32 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <limits>

typedef enum {
	GET = 0,
	POST,
	DELETE,
} e_methods;

class LocationConfig {
	public:
		LocationConfig();

		void						setIndex(const std::vector<std::string> &tokens, size_t &pos);
		void						setAllowMethods(const std::vector<std::string> &tokens, size_t &pos);
		void						setAutoindex(const std::vector<std::string> &tokens, size_t &pos);
		void						setRoot(const std::vector<std::string> &tokens, size_t &pos);
		void						setCgiPass(const std::vector<std::string> &tokens, size_t &pos);
		void						setClientUpload(const std::vector<std::string> &tokens, size_t &pos);
		void						setRedir(const std::vector<std::string> &tokens, size_t &pos);
		void						setClientBodyBufferSize(const std::vector<std::string> &tokens, size_t &pos);

		std::vector<std::string>	getIndex() const;
		std::string					getPath() const;
		std::string					getRoot() const;
		std::string					getCgiPass() const;
		std::string					getClientUpload() const;
		std::pair<int, std::string>	getRedir() const; // if first is 0, no redir
		bool						getAllowMethods(e_methods method) const;
		bool						getAutoindex() const;
		int							getClientBodyBufferSize() const;
	private:
		std::vector<std::string>	_index;
		std::string					_root;
		std::string					_cgi_pass;
		std::string					_client_upload;
		std::pair<int, std::string>	_redir;
		bool						_allow_methods[3];
		bool						_autoindex;
		int							_client_body_buffer_size;
};

LocationConfig	parseLocation(const std::vector<std::string> &tokens, size_t &pos);
