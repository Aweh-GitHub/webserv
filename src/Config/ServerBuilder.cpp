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

#include "ConfigBuilder.hpp"
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
	
	while (std::getline(file, line) && !isLineEndBracket(line))
	{
		std::string	key, value;
		std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)>::iterator handlerIt;
		void (*handler)(Server&, std::string, std::ifstream& file);

		++ConfigBuilder::LineIndex;
		if (isCommentLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		handlerIt = handlers.find(key);
		if (handlerIt == handlers.end())
			throw std::runtime_error("Error: PARSE_SERVER, at line (" + toString(ConfigBuilder::LineIndex) + ") unrecognized key \"" + key + "\".");
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
	server.SetHostIp(lineStripQuotes(value));
}

void	ServerBuilder::handleParse_ListenPort(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	server.SetListenPort(toSize(value));
}

void	ServerBuilder::handleParse_ServerDomains(Server& server, std::string value, std::ifstream& file)
{
	std::vector<std::string> domains;
	
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

	if (value != "{")
		throw std::runtime_error("Error: after 'location' key expected to get '{'");
	location = LocationBuilder::ParseLocation(file);
	server.AddLocation(location.GetName(), location);
}