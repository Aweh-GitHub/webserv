/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBuilder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 13:33:49 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/01 19:56:13 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERBUILDER_HPP
# define SERVERBUILDER_HPP

# include "Server.hpp"

class ServerBuilder
{
	private:
		ServerBuilder();
		~ServerBuilder();

	public:
		static Server	ParseServer(std::ifstream& file);
		
	private:
		static std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)>	getParseHandlers();
		static void	handleParse_HostIp(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_ListenPort(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_ServerDomains(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_PathRoot(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_IndexFiles(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_ClientMaxBodySize(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_ErrorPages(Server& server, std::string value, std::ifstream& file);
		static void	handleParse_Location(Server& server, std::string value, std::ifstream& file);
};

#endif