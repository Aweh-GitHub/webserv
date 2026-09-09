/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBuilder.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:18:23 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/04 15:05:37 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Config.hpp"
#include "ConfigBuilder.hpp"
#include "ServerBuilder.hpp"
#include "Colors.hpp"
#include "__internal__.hpp"

Config	ConfigBuilder::ParseConfig(std::string& pathConfigFile)
{
	std::ifstream	file(pathConfigFile.c_str());
	Config	config(pathConfigFile);

	if (!file.is_open())
	{
		std::cerr << RED << "Error: can't open file \"" << pathConfigFile << "\"" << RST << std::endl;
		std::exit(1);
	}

	std::string line;
	
	for (size_t i = 0; std::getline(file, line); i++)
	{
		std::string	key, value;

		if (isCommentLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		if (key != "server")
			throw std::runtime_error("Error: PARSE_CONFIG, at line (" + toString(i) + ") unrecognized key \"" + key + "\".");
		handleParse_Server(config, value, file);
	}
	
	file.close();
	return (config);
}

void	ConfigBuilder::handleParse_Server(Config& config, std::string value, std::ifstream& file)
{
	Server server;

	if (value != "{")
		throw std::runtime_error("Error: after 'server' key expected to get '{'");
	server = ServerBuilder::ParseServer(file);
	config.AddServer(server);
}
