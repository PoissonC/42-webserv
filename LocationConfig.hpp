/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 10:44:50 by yu                #+#    #+#             */
/*   Updated: 2024/09/09 21:48:56 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdlib>
#include <fcntl.h>
#include <limits>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

typedef enum {
	GET = 0,
	POST,
	DELETE,
	ERROR
} e_methods;

class LocationConfig {
public:
	LocationConfig();

	void setIndex(const std::vector< std::string > &tokens, size_t &pos);
	void setAllowMethods(const std::vector< std::string > &tokens, size_t &pos);
	void setAllAllowed();
	void setAutoindex(const std::vector< std::string > &tokens, size_t &pos);
	void setRoot(const std::vector< std::string > &tokens, size_t &pos);
	void setCgiPass(const std::vector< std::string > &tokens, size_t &pos);
	void setRedir(const std::vector< std::string > &tokens, size_t &pos);
	void setClientBodyBufferSize(const std::vector< std::string > &tokens, size_t &pos);

	std::vector< std::string > getIndex() const;
	std::string getPath() const;
	std::string getRoot() const;
	std::string getCgiPass() const;
	std::pair< int, std::string > getRedir() const;// if first is 0, no redir
	bool getAllowMethods(e_methods method) const;
	bool getAutoindex() const;
	size_t getClientBodyBufferSize() const;

private:
	std::vector< std::string > _index;
	std::string _root;
	std::string _cgi_pass;
	std::pair< int, std::string > _redir;
	bool _allow_methods[3];
	bool _autoindex;
	size_t _client_body_buffer_size;
};

LocationConfig parseLocation(const std::vector< std::string > &tokens, size_t &pos);
