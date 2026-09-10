/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 14:54:25 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/10 15:05:42 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigBuilder.hpp"
#include "LocationBuilder.hpp"
#include "__internal__.hpp"
#include <iostream>
#include <utility>

Location	LocationBuilder::ParseLocation(std::ifstream& file)
{
	Location	location;
	std::string line;
	std::map<std::string, void (*)(Location&, std::string)> handlers;

	handlers = getParseHandlers();
	while (std::getline(file, line) && !isLineEndBracket(line))
	{
		std::string	key, value;
		std::map<std::string, void (*)(Location&, std::string)>::iterator handlerIt;
		void (*handler)(Location&, std::string);

		(void)handler;
		++ConfigBuilder::LineIndex;
		if (isCommentLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		handlerIt = handlers.find(key);
		//
		// TODO: store in ConfigBuilder the line count
		//
		if (handlerIt == handlers.end())
			throw std::runtime_error("Error: PARSE_LOCATION, at line (" + toString(ConfigBuilder::LineIndex) + ") unrecognized key \"" + key + "\".");
		handler = handlerIt->second;
		(*handler)(location, value);
	}
	return (location);
}

std::map<std::string, void (*)(Location&, std::string)>	LocationBuilder::getParseHandlers()
{
	std::map<std::string, void (*)(Location&, std::string)>	handlers;

	handlers["name"]				=	&LocationBuilder::handleParse_Name;
	handlers["pathRoot"]			=	&LocationBuilder::handleParse_PathRoot;
	handlers["indexFiles"]			=	&LocationBuilder::handleParse_IndexFiles;
	handlers["pathUploadStore"]		=	&LocationBuilder::handleParse_UploadStore;
	handlers["allowedMethods"]		=	&LocationBuilder::handleParse_AllowedMethods;
	handlers["cgiExtension"]		=	&LocationBuilder::handleParse_CGIExtension;
	handlers["clientMaxBodySize"]	=	&LocationBuilder::handleParse_ClientMaxBodySize;
	handlers["autoIndex"]			=	&LocationBuilder::handleParse_AutoIndex;
	handlers["redirect"]			=	&LocationBuilder::handleParse_Redirect;
	return (handlers);
}

void	LocationBuilder::handleParse_Name(Location& location, std::string value)
{
	(void) location; (void) value;
	location.SetName(lineStripQuotes(value));
}

void	LocationBuilder::handleParse_PathRoot(Location& location, std::string value)
{
	(void) location; (void) value;
	location.SetPathRoot(lineStripQuotes(value));
}

void	LocationBuilder::handleParse_IndexFiles(Location& location, std::string value)
{
	std::vector<std::string> indexFiles;
	
	indexFiles = split(value, ',');
	for (size_t i = 0; i < indexFiles.size(); i++)
		indexFiles[i] = lineStripQuotes(indexFiles[i]);
	location.SetIndexFiles(indexFiles);
}

void	LocationBuilder::handleParse_UploadStore(Location& location, std::string value)
{
	location.SetPathUploadStore(lineStripQuotes(value));
}

//
// TODO: dictate which methods are existing/ accepted
//
void	LocationBuilder::handleParse_AllowedMethods(Location& location, std::string value)
{
	std::set<std::string> allowedMethods;
	std::vector<std::string> methods;

	(void) location; (void) value;
	methods = split(value, ',');
	for (size_t i = 0; i < methods.size(); i++)
	{
		std::string methodCleaned;
		std::pair<std::set<std::string>::iterator, bool> insertResult;
		
		methodCleaned = lineStripQuotes(methods[i]);
		insertResult = allowedMethods.insert(methodCleaned);
		std::cout << "method Cleaned: " << methodCleaned << std::endl;
		if (!insertResult.second)
			throw std::invalid_argument("Error: duplicate method " + methods[i] + " in location.");
	}
	location.SetAllowedMethods(allowedMethods);	
}

void	LocationBuilder::handleParse_CGIExtension(Location& location, std::string value)
{
	std::vector<std::string> cgi;
	
	(void) location; (void) value;
	cgi = split(value, ',');
	for (size_t i = 0; i < cgi.size(); i++)
	{
		cgi[i] = lineStripQuotes(cgi[i]);
	}
	location.AddCGIExtension(cgi[0], cgi[1]);
}

void	LocationBuilder::handleParse_ClientMaxBodySize(Location& location, std::string value)
{
	(void) location; (void) value;
	location.SetClientMaxBodySize(toSize(value));
}

void	LocationBuilder::handleParse_AutoIndex(Location& location, std::string value)
{
	std::string valueCleaned;
	bool result;
	
	(void) location; (void) value;
	valueCleaned = lineStripQuotes(value);
	if (valueCleaned == "true")
		result = true;
	else if (valueCleaned == "false")
		result = false;
	else
		throw std::runtime_error("Error: for key 'autoindex' expected either \"true\" or \"false\"");
	location.SetAutoIndex(result);
}

void	LocationBuilder::handleParse_Redirect(Location& location, std::string value)
{
	(void) location; (void) value;
	std::cout << "handleParse_Redirect - Not implemented - value: " << value << std::endl;
}