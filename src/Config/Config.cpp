/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:18:33 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/10 14:54:20 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

Config::Config() : _allServers(), _isInitialized(false)
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
	std::cout << "CONFIG FILE: " << std::endl;
	for (size_t i = 0; i < this->_allServers.size(); i++)
	{
		this->_allServers[i].Print();
	}
	
}

void	Config::ToggleInitialized()
{
	this->_isInitialized = true;
}

bool	Config::GetIsInitialized()
{
	return (this->_isInitialized);
}