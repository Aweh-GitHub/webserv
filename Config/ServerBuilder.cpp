/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBuilderBuilder.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 13:34:42 by thantoni          #+#    #+#             */
/*   Updated: 2026/08/30 13:36:16 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBuilder.hpp"
#include "Colors.hpp"
#include "__internal__.hpp"
#include "LocationBuilder.hpp"
#include <fstream>
#include <iostream>

Server	ServerBuilder::ParseServer(std::ifstream& file)
{
	Server server;
	std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)> handlers;

	handlers = getParseHandlers();

	std::string line;
	
	for (size_t i = 0; std::getline(file, line) && !isLineEndBracket(line); i++)
	{
		std::string	key, value;
		std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)>::iterator handlerIt;
		void (*handler)(Server&, std::string, std::ifstream& file);

		if (isCommentLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		handlerIt = handlers.find(key);
		if (handlerIt == handlers.end())
			throw std::runtime_error("Error: PARSE_SERVER, at line (" + toString(i) + ") unrecognized key \"" + key + "\".");
		handler = handlerIt->second;
		(*handler)(server, value, file);
	}
	return (server);
}

std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)>	ServerBuilder::getParseHandlers()
{
	std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)>	handlers;

	handlers["hostIp"]				=	&ServerBuilder::handleParse_HostIp;
	handlers["listenPort"]			=	&ServerBuilder::handleParse_ListenPort;
	handlers["serverDomains"]		=	&ServerBuilder::handleParse_ServerDomains;
	handlers["pathRoot"]			=	&ServerBuilder::handleParse_PathRoot;
	handlers["indexFiles"]			=	&ServerBuilder::handleParse_IndexFiles;
	handlers["clientMaxBodySize"]	=	&ServerBuilder::handleParse_ClientMaxBodySize;
	handlers["errorPage"]			=	&ServerBuilder::handleParse_ErrorPages;
	handlers["location"]			=	&ServerBuilder::handleParse_Location;
	return (handlers);
}

//
// TODO: verif format 0.0.0.0
//
void	ServerBuilder::handleParse_HostIp(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	std::cout << "handleParse_HostIp: " << value << std::endl;
	server.SetHostIp(lineStripQuotes(value));
}

void	ServerBuilder::handleParse_ListenPort(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	std::cout << "handleParse_ListenPort: " << value << std::endl;
	server.SetListenPort(toSize(value));
}

void	ServerBuilder::handleParse_ServerDomains(Server& server, std::string value, std::ifstream& file)
{
	std::vector<std::string> domains;
	
	std::cout << "handleParse_ServerDomains: " << value << std::endl;
	(void)file;
	domains = split(value, ',');
	for (size_t i = 0; i < domains.size(); i++)
		domains[i] = lineStripQuotes(domains[i]);
	server.SetServerDomains(domains);
}

void	ServerBuilder::handleParse_PathRoot(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	server.SetPathRoot(lineStripQuotes(value));
}

//
// TODO: do i have to check '.htm' / '.html' ?
//
void	ServerBuilder::handleParse_IndexFiles(Server& server, std::string value, std::ifstream& file)
{
	std::vector<std::string> indexFiles;
	
	std::cout << "handleParse_IndexFiles: " << value << std::endl;
	(void)file;
	indexFiles = split(value, ',');
	for (size_t i = 0; i < indexFiles.size(); i++)
		indexFiles[i] = lineStripQuotes(indexFiles[i]);
	server.SetIndexFiles(indexFiles);
}

void	ServerBuilder::handleParse_ClientMaxBodySize(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	server.SetClientMaxBodySize(toSize(value));
}

//
// TODO: verif: firsts args are only size_t verif: last arg is only string
// TODO: verif: no duplicate
//
void	ServerBuilder::handleParse_ErrorPages(Server& server, std::string value, std::ifstream& file)
{
	std::vector<std::string> args;
	std::string errorPage;

	std::cout << "handleParse_ErrorPages: " << value << std::endl;
	(void)file;
	args = split(value, ',');
	errorPage = lineStripQuotes(*(--args.end()));
	for (size_t i = 0; i < args.size() - 1; i++)
	{
		size_t errorCode;

		errorCode = toSize(args[i]);
		server.AddErrorPage(errorCode, errorPage);
	}
}

void	ServerBuilder::handleParse_Location(Server& server, std::string value, std::ifstream& file)
{
	Location location;

	std::cout << "handleParse_Location: " << value << std::endl;
	if (value != "{")
		throw std::runtime_error("Error: after 'location' key expected to get '{'");
	location = LocationBuilder::ParseLocation(file);
	server.AddLocation(location.GetName(), location);
}