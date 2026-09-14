/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:18:33 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/14 20:52:35 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>

Config::Config() : _allServers(), _isInitialized(false) { }

Config::~Config() { }

void	Config::Print() const
{
	std::cout << "CONFIG FILE: " << std::endl;
	for (size_t i = 0; i < this->_allServers.size(); i++)
		this->_allServers[i].Print();
}

const std::vector<Server>	Config::GetAllServers() { return (this->_allServers); }
void	Config::AddServer(const Server server) { this->_allServers.push_back(server); }

void	Config::ToggleInitialized() { this->_isInitialized = true; }
bool	Config::GetIsInitialized() { return (this->_isInitialized); }

const Server*	Config::TryFindServer(const std::string& hostIp, const size_t listenPort, const std::string& serverDomain) const
{
	for (size_t i = 0; i < this->_allServers.size(); i++)
	{
		Server currentServer;

		currentServer = this->_allServers[i];
		if (currentServer.GetHostIp() != hostIp)
			continue;
		if (currentServer.GetListenPort() != listenPort)
			continue;
		if (std::find(currentServer.GetServerDomains().begin(), currentServer.GetServerDomains().end(), serverDomain) == currentServer.GetServerDomains().end())
			continue;
		return (&this->_allServers[i]);
	}
	return (NULL);
}