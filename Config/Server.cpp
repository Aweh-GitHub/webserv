/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 16:20:46 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/01 19:53:30 by thantoni         ###   ########.fr       */
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

//
// TODO: handle dups and edge cases
//
void	Server::AddErrorPage(size_t errorCode, std::string pathErrorPage)
{
	this->_errorPages[errorCode] = pathErrorPage;
}

//
// TODO: handle dups and edge cases
//
void	Server::AddLocation(std::string locationName, Location location)
{
	this->_locations[locationName] = location;
}