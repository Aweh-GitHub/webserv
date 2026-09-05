/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:18:33 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/01 20:31:20 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

Config::Config(const std::string& pathConfigFile) : _pathConfigFile(pathConfigFile), _allServers()
{
}

Config::~Config()
{
	
}

const std::vector<Server>	Config::GetAllServers()
{
	return (this->_allServers);
}

void	Config::AddServer(const Server server)
{
	this->_allServers.push_back(server);
}

void	Config::Print() const
{
	std::cout << "file: " << this->_pathConfigFile << std::endl;
	for (size_t i = 0; i < this->_allServers.size(); i++)
	{
		this->_allServers[i].Print();
	}
	
}