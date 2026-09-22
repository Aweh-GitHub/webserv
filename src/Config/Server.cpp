/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 16:20:46 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/22 16:11:44 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <exception>
#include "Server.hpp"
#include "Colors.hpp"
#include "__internal__.hpp"

Server::Server() : _hostIp(), _listenPort(), _serverDomains(), _pathRoot(), _indexFiles(), _clientMaxBodySize(1048576), _errorPages(), _locations() {	}

Server::~Server() { }


void	Server::Print() const
{
	std::cout	<< "\n"
				<< "___________________________________________\n"
				<< "\n"
				<< "               S E R V E R\n"
				<< "___________________________________________\n"
				<< "\n"
				<< "\n"
				<< "[>]   Host IP\t\t\t: \"" << this->_hostIp				<< "\"\n"
				<< "_________________________________\n\n"
				<< "[>]   Listen Port\t\t: " << this->_listenPort			<< "\n"
				<< "_________________________________\n\n"
				<< "[>]   Server Domains\t\t: ";
	for (size_t i = 0; i < this->_serverDomains.size(); ++i)
	{
		std::cout << "\"" << this->_serverDomains[i] << "\"";
		if (i + 1 < this->_serverDomains.size())
			std::cout << ", ";
	}
	std::cout	<< "\n_________________________________\n\n"
				<< "[>]   Path Root\t\t\t: \"" << this->_pathRoot				<< "\"\n"
				<< "_________________________________\n\n"
				<< "[>]   Index Files\t\t: ";
	for (size_t i = 0; i < this->_indexFiles.size(); ++i)
	{
		std::cout << "\"" << this->_indexFiles[i] << "\"";
		if (i + 1 < this->_indexFiles.size())
			std::cout << ", ";
	}
	std::cout	<< "\n_________________________________\n\n"
				<< "[>]   Client Max Body Size\t: " << this->_clientMaxBodySize << "\n"
				<< "_________________________________\n\n"
				<< "[>]   Error Pages\t\t: ";
	for (std::map<size_t, std::string>::const_iterator errorPageIt = this->_errorPages.begin(); errorPageIt != this->_errorPages.end(); ++errorPageIt)
	{
		std::cout << "{ " << errorPageIt->first << " : \"" << errorPageIt->second << "\" }  ";
	}
	std::cout	<< "\n_________________________________\n"
				<< "[v]   Locations\t\t\t:\n";
	for (std::map<std::string, Location>::const_iterator locationIt = this->_locations.begin(); locationIt != this->_locations.end(); ++locationIt)
	{
		locationIt->second.Print();
	}
	std::cout		<< "\n_________________________________\n\n"
					<< std::endl;
}

void	Server::SetHostIp(std::string hostIp) { this->_hostIp = hostIp; }

void	Server::SetListenPort(size_t listenPort) { this->_listenPort = listenPort; }

void	Server::SetServerDomains(std::vector<std::string> serverDomains) { this->_serverDomains = serverDomains; }

void	Server::SetPathRoot(std::string pathRoot) { this->_pathRoot = pathRoot; }

void	Server::SetIndexFiles(std::vector<std::string> indexFiles) { this->_indexFiles = indexFiles; }

void	Server::SetClientMaxBodySize(size_t clientMaxBodySize) { this->_clientMaxBodySize = clientMaxBodySize; }

void	Server::AddErrorPage(size_t errorCode, std::string pathErrorPage)
{
	if (this->_errorPages.find(errorCode) != this->_errorPages.end())
		throwLineError("Duplicate errorPage: " + toString(errorCode));
	this->_errorPages[errorCode] = pathErrorPage;
}

void	Server::AddLocation(std::string locationName, Location location)
{
	if (this->_locations.find(locationName) != this->_locations.end())
		throwLineError("Duplicate location: " + locationName);
	this->_locations[locationName] = location;
}

const std::string						Server::GetHostIp() const { return (this->_hostIp); }
size_t									Server::GetListenPort() const { return (this->_listenPort); }
const std::vector<std::string>&			Server::GetServerDomains() const { return (this->_serverDomains); }
const std::string						Server::GetPathRoot() const { return (this->_pathRoot); }
const std::vector<std::string>&			Server::GetIndexFiles() const { return (this->_indexFiles); }
size_t									Server::GetClientMaxBodySize() const { return (this->_clientMaxBodySize); }
const std::map<size_t, std::string>&	Server::GetErrorPages() const { return (this->_errorPages); }
const std::map<std::string, Location>&	Server::GetLocations() const { return (this->_locations); }

// const Location*	Server::TryFindLocation(const std::string &name) const
// {
// 	std::map<std::string, Location>::const_iterator	it;
	
// 	it = this->_locations.find(name);
// 	if (it != this->_locations.end())
// 		return &(it->second);
// 	return (NULL);
// }

const Location* Server::TryFindLocation(const std::string &name) const
{
	std::map<std::string, Location>::const_iterator it;
	const Location *best = NULL;
	size_t bestLength = 0;

	for (it = this->_locations.begin(); it != this->_locations.end(); ++it)
	{
		const std::string &location = it->first;

		if (name.compare(0, location.length(), location) != 0)
			continue;

		// Make sure /foo does not match /foobar
		if (location != "/" &&
			name.length() > location.length() &&
			name[location.length()] != '/')
			continue;

		if (location.length() > bestLength)
		{
			best = &it->second;
			bestLength = location.length();
		}
	}

	return (best);
}