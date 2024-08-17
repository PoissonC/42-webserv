/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yu <yu@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 18:03:41 by yu                #+#    #+#             */
/*   Updated: 2024/05/30 19:03:43 by yu               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

static std::string readFile(const std::string &filename) {
    std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filename);
	}
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static std::vector<std::string> tokenize(const std::string &input) {
    std::istringstream stream(input);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
		std::vector<std::string> tmp;
		size_t pos = 0;
		while (pos < token.size()) {
			if (token[pos] == ';') {
				tmp.push_back(";");
				pos++;
			}
			else if (token[pos] == '{') {
				tmp.push_back("{");
				pos++;
			}
			else if (token[pos] == '}') {
				tmp.push_back("}");
				pos++;
			}
			else {
				size_t end = token.find_first_of(";{}", pos);
				tmp.push_back(token.substr(pos, end - pos));
				pos = end;
			}
		}
		for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++) {
			if (*it != "")
				tokens.push_back(*it);
		}
	}
	return tokens;
}

static void parseConfig(const std::vector<std::string> &tokens, std::vector<ServerConfig> &servers) {
	size_t pos = 0;
	while (pos < tokens.size()) {
		if (tokens[pos++] == "server") {
			if (tokens[pos++] != "{") {
				throw std::runtime_error("Expected '{' after 'server'");
			}
			servers.push_back(parseServer(tokens, pos));
		}
		else {
			throw std::runtime_error("Expected 'server' at the outermost level");
		}
	}
}

void	parse(std::vector<Settings> & settings, const std::string & filename) {
	std::vector<std::string> tokens = tokenize(readFile(filename));
	std::cout << "Start parsing config" << std::endl;
	std::vector<ServerConfig> servers;
	parseConfig(tokens, servers);
	std::cout << "Finish parsing config" << std::endl;
	for (std::vector<ServerConfig>::iterator Server_it = servers.begin(); Server_it != servers.end(); Server_it++) {
		// add new server if the listening host:port is not in the settings
		std::vector<Settings>::iterator Settings_it = settings.begin();
		for (; Settings_it != settings.end(); Settings_it++) {
			if (Settings_it->getListen() == Server_it->getListen()) {
				break;
			}
		}
		if (Settings_it == settings.end()) {
			Settings s(*Server_it);
			settings.push_back(s);
		}
		else {
			Settings_it->addServer(*Server_it);
		}
	}
}