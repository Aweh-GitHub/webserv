/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:18:33 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/15 17:08:42 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

static std::string lowerAscii(const std::string &value)
{
	std::string result = value;

	for (size_t i = 0; i < result.length(); ++i)
		result[i] = static_cast<char>(std::tolower(
			static_cast<unsigned char>(result[i])));
	return (result);
}

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
	const std::string normalizedDomain = lowerAscii(serverDomain);

	for (size_t i = 0; i < this->_allServers.size(); i++)
	{
		Server currentServer;
		std::vector<std::string> serverDomains;

		currentServer = this->_allServers[i];
		if (currentServer.GetHostIp() != hostIp)
			continue;
		if (currentServer.GetListenPort() != listenPort)
			continue;
		serverDomains = currentServer.GetServerDomains();
		bool domainMatches = false;
		for (size_t j = 0; j < serverDomains.size(); ++j)
		{
			if (lowerAscii(serverDomains[j]) == normalizedDomain)
			{
				domainMatches = true;
				break;
			}
		}
		if (!domainMatches)
			continue;
		return (&this->_allServers[i]);
	}
	return (NULL);
}
