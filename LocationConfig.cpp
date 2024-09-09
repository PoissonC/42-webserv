/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 12:34:34 by yu                #+#    #+#             */
/*   Updated: 2024/09/09 21:49:39 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include "constants.hpp"

LocationConfig::LocationConfig() : _root("./"), _cgi_pass(), _redir(std::make_pair(0, "")), _autoindex(false), _client_body_buffer_size(8 * 1024) {
	for (int i = 0; i < 3; i++)
		_allow_methods[i] = false;
}
#include <iostream>
void LocationConfig::setAllowMethods(const std::vector< std::string > &tokens, size_t &pos) {
	while (tokens[pos] != ";" && pos < tokens.size()) {
		if (tokens[pos] == "GET")
			_allow_methods[GET] = true;
		else if (tokens[pos] == "POST")
			_allow_methods[POST] = true;
		else if (tokens[pos] == "DELETE")
			_allow_methods[DELETE] = true;
		else
			throw std::runtime_error("Invalid method");
		pos++;
	}
	if (pos == tokens.size())
		throw std::runtime_error("Expected ';' after 'allow_methods'");
	pos++;// skip ';'
}

void LocationConfig::setRoot(const std::vector< std::string > &tokens, size_t &pos) {
	_root = tokens[pos++];
	if (tokens[pos++] != ";")
		throw std::runtime_error("Expected ';' after root");
}

void LocationConfig::setIndex(const std::vector< std::string > &tokens, size_t &pos) {
	while (tokens[pos] != ";" && pos < tokens.size()) {
		// int fd = open(tokens[pos].c_str(), O_RDONLY);
		// if (fd == -1)
		// 	throw std::runtime_error("Index file does not exist");
		// close(fd);
		_index.push_back(tokens[pos++]);
	}
	if (pos == tokens.size())
		throw std::runtime_error("Expected ';' after index");
	pos++;// skip ';'
}

void LocationConfig::setCgiPass(const std::vector< std::string > &tokens, size_t &pos) {
	_cgi_pass = tokens[pos++];
	if (tokens[pos++] != ";")
		throw std::runtime_error("Expected ';' after cgi_pass");
}

void LocationConfig::setRedir(const std::vector< std::string > &tokens, size_t &pos) {
	if (tokens[pos] == "301")
		_redir.first = MOVED_PERMANENTLY;
	else if (tokens[pos] == "302")
		_redir.first = FOUND;
	else if (tokens[pos] == "303")
		_redir.first = SEE_OTHER;
	else if (tokens[pos] == "307")
		_redir.first = TEMPORARY_REDIRECT;
	else if (tokens[pos] == "308")
		_redir.first = PERMANENT_REDIRECT;
	else
		throw std::runtime_error("Invalid return code");
	if (pos + 1 == tokens.size() || tokens[pos + 1] == ";")
		throw std::runtime_error("Expected return url");
	_redir.second = tokens[++pos];
	if (tokens[++pos] != ";")
		throw std::runtime_error("Expected ';' after return");
	pos++;// skip ';'
}

void LocationConfig::setAutoindex(const std::vector< std::string > &tokens, size_t &pos) {
	if (tokens[pos] == "on")
		_autoindex = true;
	else if (tokens[pos] == "off")
		_autoindex = false;
	else
		throw std::runtime_error("Invalid autoindex");
	if (tokens[++pos] != ";")
		throw std::runtime_error("Expected ';' after autoindex");
	pos++;// skip ';'
}

void LocationConfig::setClientBodyBufferSize(const std::vector< std::string > &tokens, size_t &pos) {
	_client_body_buffer_size = 0;
	size_t i = 0;
	if (tokens[pos][0] == ';')
		throw std::runtime_error("Invalid client_body_buffer_size");
	while (i < tokens[pos].size()) {
		if (isdigit(tokens[pos][i])) {
			_client_body_buffer_size *= 10;
			_client_body_buffer_size += tokens[pos][i] - '0';
			i++;
		}
		else
			break;
	}
	if (i == 0)
		throw std::runtime_error("Invalid client_body_buffer_size");
	if (i == tokens[pos].size() - 1) {
		if (tokens[pos][i] == 'K' || tokens[pos][i] == 'k') {
			if (_client_body_buffer_size > (size_t)std::numeric_limits< int >::max() / 1024)
				throw std::runtime_error("Invalid client_body_buffer_size");
			_client_body_buffer_size *= 1024;
		} else if (tokens[pos][i] == 'M' || tokens[pos][i] == 'm') {
			if (_client_body_buffer_size > (size_t)std::numeric_limits< int >::max() / (1024 * 1024))
				throw std::runtime_error("Invalid client_body_buffer_size");
			_client_body_buffer_size *= 1024 * 1024;
		} else {
			throw std::runtime_error("Invalid client_body_buffer_size");
		}
	}	else {
		throw std::runtime_error("Invalid client_body_buffer_size");
	}
	if (tokens[++pos] != ";")
		throw std::runtime_error("Expected ';' after client_body_buffer_size");
	pos++;// skip ';'
}

std::vector< std::string > LocationConfig::getIndex() const {
	return _index;
}

std::string LocationConfig::getRoot() const {
	return _root;
}

std::string LocationConfig::getCgiPass() const {
	return _cgi_pass;
}

std::pair< int, std::string > LocationConfig::getRedir() const {
	return _redir;
}

bool LocationConfig::getAutoindex() const {
	return _autoindex;
}

bool LocationConfig::getAllowMethods(e_methods method) const {
	if (method == ERROR)
		return false;
	return _allow_methods[method];
}

size_t LocationConfig::getClientBodyBufferSize() const {
	return _client_body_buffer_size;
}

void LocationConfig::setAllAllowed() {
	_allow_methods[0] = _allow_methods[1] = _allow_methods[2] = true;
}


LocationConfig parseLocation(const std::vector< std::string > &tokens, size_t &pos) {
	LocationConfig location;
	while (tokens[pos] != "}" && pos < tokens.size()) {
		if (tokens[pos] == "index") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected index file");
			location.setIndex(tokens, ++pos);
		} else if (tokens[pos] == "root") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected root directory");
			location.setRoot(tokens, ++pos);
		} else if (tokens[pos] == "allow_methods") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected methods");
			location.setAllowMethods(tokens, ++pos);
		} else if (tokens[pos] == "autoindex") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected autoindex value");
			location.setAutoindex(tokens, ++pos);
		} else if (tokens[pos] == "cgi_pass") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected cgi_pass");
			location.setCgiPass(tokens, ++pos);
		} else if (tokens[pos] == "client_max_body_size") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected client_body_buffer_size");
			location.setClientBodyBufferSize(tokens, ++pos);
		} else if (tokens[pos] == "return") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected return code");
			location.setRedir(tokens, ++pos);
		} else
			throw std::runtime_error("Invalid location directive '" + tokens[pos] + "'");
	}
	if (pos == tokens.size())
		throw std::runtime_error("Expected '}' after location block");
	pos++;// skip '}'
	if (!location.getAllowMethods(GET) && !location.getAllowMethods(POST) && !location.getAllowMethods(DELETE))
		location.setAllAllowed();		
	return location;
}
