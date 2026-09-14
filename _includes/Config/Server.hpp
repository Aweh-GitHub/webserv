/* ************************************************************************* */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 16:20:05 by thantoni          #+#    #+#             */
/*   Updated: 2026/08/27 17:08:47 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <string>
# include "Location.hpp"

class Server
{
	public:
		Server();
		~Server();

	public:
		void	Print() const;

		const std::string						GetHostIp() const;
		size_t									GetListenPort() const;
		const std::vector<std::string>&			GetServerDomains() const;
		const std::string						GetPathRoot() const;
		const std::vector<std::string>&			GetIndexFiles() const;
		size_t									GetClientMaxBodySize() const;
		const std::map<size_t, std::string>&	GetErrorPages() const;
		const std::map<std::string, Location>&	GetLocations() const;

		void	SetHostIp(const std::string hostIp);
		void	SetListenPort(const size_t listenPort);
		void	SetServerDomains(const std::vector<std::string> serverDomains);
		void	SetPathRoot(const std::string pathRoot);
		void	SetIndexFiles(const std::vector<std::string> indexFiles);
		void	SetClientMaxBodySize(const size_t clientMaxBodySize);
		void	AddErrorPage(const size_t errorCode, const std::string pathErrorPage);
		void	AddLocation(const std::string locationName, const Location location);

		const Location*	TryFindLocation(const std::string &name) const;
	private:
		std::string								_hostIp;
		size_t									_listenPort;
		std::vector<std::string>				_serverDomains; 
		std::string								_pathRoot;
		std::vector<std::string>				_indexFiles; // indexes, files fallback order: left to right (0 -> ...)
		size_t									_clientMaxBodySize;
		std::map<size_t, std::string>			_errorPages; // map<error_code, path_to_file_html>
		std::map<std::string, Location>			_locations; // map<Location.name, Location>
};

#endif