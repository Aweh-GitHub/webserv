/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBuilder.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:18:23 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/10 15:10:32 by thantoni         ###   ########.fr       */
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

int	ConfigBuilder::LineIndex = 1;

Config	ConfigBuilder::ParseConfig(std::string& pathConfigFile)
{
	std::ifstream	file(pathConfigFile.c_str());
	Config	config;

	if (!file.is_open())
	{
		std::cerr << RED << "Error: can't open file \"" << pathConfigFile << "\"" << RST << std::endl;
		std::exit(1);
	}

	std::string line;
	
	ConfigBuilder::LineIndex = 1;
	while (std::getline(file, line))
	{
		std::string	key, value;

		++ConfigBuilder::LineIndex;
		if (isCommentLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		if (key != "server")
			throw std::runtime_error("Error: PARSE_CONFIG, at line (" + toString(ConfigBuilder::LineIndex) + ") unrecognized key \"" + key + "\".");
		handleParse_Server(config, value, file);
	}
	
	file.close();
	config.ToggleInitialized();
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
