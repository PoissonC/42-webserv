/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 12:34:34 by yu                #+#    #+#             */
/*   Updated: 2024/05/30 18:14:27 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _root(), _cgi_pass(), _client_upload("forbidden"), _redir(std::make_pair (0, "")), _autoindex(false), _client_body_buffer_size(8 * 1024) {
	for (int i = 0; i < 3; i++)
		_allow_methods[i] = true;
}

void	LocationConfig::setAllowMethods(const std::vector<std::string> &tokens, size_t &pos) {
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
	pos++; // skip ';'
}

void	LocationConfig::setRoot(const std::vector<std::string> &tokens, size_t &pos) {
	_root = tokens[pos++];
	if (tokens[pos++] != ";")
		throw std::runtime_error("Expected ';' after root");
}

void	LocationConfig::setIndex(const std::vector<std::string> &tokens, size_t &pos) {
	while (tokens[pos] != ";" && pos < tokens.size()) {
		// int fd = open(tokens[pos].c_str(), O_RDONLY);
		// if (fd == -1)
		// 	throw std::runtime_error("Index file does not exist");
		// close(fd);
		_index.push_back(tokens[pos++]);
	}
	if (pos == tokens.size())
		throw std::runtime_error("Expected ';' after index");
	pos++; // skip ';'
}

void	LocationConfig::setCgiPass(const std::vector<std::string> &tokens, size_t &pos) {
	_cgi_pass = tokens[pos++];
	if (tokens[pos++] != ";")
		throw std::runtime_error("Expected ';' after cgi_pass");
}

void	LocationConfig::setClientUpload(const std::vector<std::string> &tokens, size_t &pos) {
	if (_client_upload != "forbidden")
		throw std::runtime_error("client_upload already set");
	_client_upload = tokens[pos++];
	if (tokens[pos++] != ";")
		throw std::runtime_error("Expected ';' after client_upload");
}

void	LocationConfig::setRedir(const std::vector<std::string> &tokens, size_t &pos) {
	if (tokens[pos] == "301")
		_redir.first = 301;
	else if (tokens[pos] == "302")
		_redir.first = 302;
	else if (tokens[pos] == "303")
		_redir.first = 303;
	else if (tokens[pos] == "307")
		_redir.first = 307;
	else if (tokens[pos] == "308")
		_redir.first = 308;
	else
		throw std::runtime_error("Invalid return code");
	if (pos + 1 == tokens.size())
		throw std::runtime_error("Expected return url");
	_redir.second = tokens[++pos];
	if (tokens[++pos] != ";")
		throw std::runtime_error("Expected ';' after return");
	pos++; // skip ';'
}

void	LocationConfig::setAutoindex(const std::vector<std::string> &tokens, size_t &pos) {
	if (tokens[pos] == "on")
		_autoindex = true;
	else if (tokens[pos] == "off")
		_autoindex = false;
	else
		throw std::runtime_error("Invalid autoindex");
	if (tokens[++pos] != ";")
		throw std::runtime_error("Expected ';' after autoindex");
	pos++; // skip ';'
}

void	LocationConfig::setClientBodyBufferSize(const std::vector<std::string> &tokens, size_t &pos) {
	_client_body_buffer_size = 0;
	size_t i = 0;
	for (; i < tokens[pos].size(); i++) {
		if (isdigit(tokens[pos][i])) {
			_client_body_buffer_size *= 10;
			_client_body_buffer_size += tokens[pos][i] - '0';
			if (_client_body_buffer_size < 0)
				throw std::runtime_error("Invalid client_body_buffer_size");
		}
	}
	if (i == 0)
		throw std::runtime_error("Invalid client_body_buffer_size");
	if (i != tokens[pos].size()) {
		if (tokens[pos][i] == 'K' || tokens[pos][i] == 'k') {
			if (_client_body_buffer_size > std::numeric_limits<int>::max() / 1024)
				throw std::runtime_error("Invalid client_body_buffer_size");
			_client_body_buffer_size *= 1024;
		}
		else if (tokens[pos][i] == 'M' || tokens[pos][i] == 'm') {
			if (_client_body_buffer_size > std::numeric_limits<int>::max() / (1024 * 1024))
				throw std::runtime_error("Invalid client_body_buffer_size");
			_client_body_buffer_size *= 1024 * 1024;
		}
		else {
			throw std::runtime_error("Invalid client_body_buffer_size");
		}
	}
	if (tokens[++pos] != ";")
		throw std::runtime_error("Expected ';' after client_body_buffer_size");
	pos++; // skip ';'
}

std::vector<std::string>	LocationConfig::getIndex() const {
	return _index;
}

std::string	LocationConfig::getRoot() const {
	return _root;
}

std::string	LocationConfig::getCgiPass() const {
	return _cgi_pass;
}

std::string	LocationConfig::getClientUpload() const {
	return _client_upload;
}

std::pair<int, std::string>	LocationConfig::getRedir() const {
	return _redir;
}

bool	LocationConfig::getAutoindex() const {
	return _autoindex;
}

bool	LocationConfig::getAllowMethods(e_methods method) const {
	return _allow_methods[method];
}

int		LocationConfig::getClientBodyBufferSize() const {
	return _client_body_buffer_size;
}

LocationConfig	parseLocation(const std::vector<std::string> &tokens, size_t &pos) {
	LocationConfig location;
	while (tokens[pos] != "}" && pos < tokens.size()) {
		if (tokens[pos] == "index") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected index file");
			location.setIndex(tokens, ++pos);
		}
		else if (tokens[pos] == "root") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected root directory");
			location.setRoot(tokens, ++pos);
		}
		else if (tokens[pos] == "allow_methods") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected methods");
			location.setAllowMethods(tokens, ++pos);
		}
		else if (tokens[pos] == "autoindex") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected autoindex value");
			location.setAutoindex(tokens, ++pos);
		}
		else if (tokens[pos] == "cgi_pass") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected cgi_pass");
			location.setCgiPass(tokens, ++pos);
		}
		else if (tokens[pos] == "client_max_body_size") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected client_body_buffer_size");
			location.setClientBodyBufferSize(tokens, ++pos);
		}
		else if (tokens[pos] == "return") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected return code");
			location.setRedir(tokens, ++pos);
		}
		else if (tokens[pos] == "client_upload") {
			if (pos + 1 == tokens.size())
				throw std::runtime_error("Expected client_upload directory");
			location.setClientUpload(tokens, ++pos);
		}
		else 
			throw std::runtime_error("Invalid location directive '" + tokens[pos] + "'");
	}
	if (pos == tokens.size())
		throw std::runtime_error("Expected '}' after location block");
	pos++; // skip '}'
	return location;
}