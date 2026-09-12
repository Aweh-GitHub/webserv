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
#include <exception>

Server	ServerBuilder::ParseServer(std::ifstream& file)
{
	Server server;
	std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)> handlers;

	handlers = getParseHandlers();

	std::string line;
	
	while (std::getline(file, line))
	{
		std::string	key, value;
		std::map<std::string, void (*)(Server&, std::string, std::ifstream& file)>::iterator handlerIt;
		void (*handler)(Server&, std::string, std::ifstream& file);

		++ConfigBuilder::LineIndex;
		line = lineTrimSpaces(line);
		if (isLineEndBracket(line))
			break;
		if (isSkipLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		handlerIt = handlers.find(key);
		if (handlerIt == handlers.end())
			throwLineError("Error: PARSE_SERVER, at line (" + toString(ConfigBuilder::LineIndex) + ") unrecognized key \"" + key + "\".");
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

void	ServerBuilder::handleParse_HostIp(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	value = lineStripQuotes(value);
	verifyIP(value);
	server.SetHostIp(value);
}

void	ServerBuilder::handleParse_ListenPort(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	server.SetListenPort(parseAndVerifyPort(value));
}

void	ServerBuilder::handleParse_ServerDomains(Server& server, std::string value, std::ifstream& file)
{
	std::vector<std::string> domains;
	
	(void)file;
	domains = splitValue(value, ',');
	for (size_t i = 0; i < domains.size(); i++)
	{
		domains[i] = lineStripQuotes(domains[i]);
		verifyDomain(domains[i]);
	}
	server.SetServerDomains(domains);
}

void	ServerBuilder::handleParse_PathRoot(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	value = lineStripQuotes(value);
	verifyDirPath(value);
	server.SetPathRoot(value);
}

void	ServerBuilder::handleParse_IndexFiles(Server& server, std::string value, std::ifstream& file)
{
	std::vector<std::string> rawFiles;
	std::vector<std::string> indexFiles;
	
	(void)file;
	rawFiles = splitValue(value, ',');
	if (rawFiles.empty())
        throwLineError("Invalid value, cannot be empty");
	for (size_t i = 0; i < rawFiles.size(); i++)
	{
		std::string indexFile;

		indexFile = lineStripQuotes(rawFiles[i]);
		verifyIndexFileName(indexFile);
		indexFiles.push_back(indexFile);
	}
	server.SetIndexFiles(indexFiles);
}

void	ServerBuilder::handleParse_ClientMaxBodySize(Server& server, std::string value, std::ifstream& file)
{
	(void)file;
	server.SetClientMaxBodySize(parseAndVerifyMaxBodySize(value));
}

void	ServerBuilder::handleParse_ErrorPages(Server& server, std::string value, std::ifstream& file)
{
	std::string rawPath;
	std::string errorPage;
	std::vector<std::string> args;

    (void)file;
    args = splitValue(value, ',');
    if (args.size() < 2)
        throwLineError("Wrong value, must be:    errorPage:<code>,[code]...,<path>");
    rawPath = args.back();
    verifyQuotesSanity(rawPath);
    errorPage = lineStripQuotes(rawPath);
    verifyURL(errorPage);
    for (size_t i = 0; i < args.size() - 1; ++i)
    {
        size_t errorCode;
		
		errorCode = parseAndVerifyErrorCode(args[i]);
        server.AddErrorPage(errorCode, errorPage);
    }
}

void	ServerBuilder::handleParse_Location(Server& server, std::string value, std::ifstream& file)
{
	Location location;

	if (value != "{")
		throwLineError("After 'location' key expected to get '{'");
	location = LocationBuilder::ParseLocation(file);
	server.AddLocation(location.GetName(), location);
}