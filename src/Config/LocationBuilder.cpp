/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 14:54:25 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/12 16:26:26 by thantoni         ###   ########.fr       */
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
	while (std::getline(file, line))
	{
		std::string	key, value;
		std::map<std::string, void (*)(Location&, std::string)>::iterator handlerIt;
		void (*handler)(Location&, std::string);
		
		++ConfigBuilder::LineIndex;
		if (isLineEndBracket(line))
			break;
		if (isSkipLine(line))
			continue;
		lineParseKeyValue(line, key, value);
		handlerIt = handlers.find(key);
		if (handlerIt == handlers.end())
			throwLineError("PARSE_LOCATION, at line (" + toString(ConfigBuilder::LineIndex) + ") unrecognized key \"" + key + "\".");
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
	std::string	name;
	
	(void) location; (void) value;
	name = lineStripQuotes(value);
	verifyURL(name);
	location.SetName(name);
}

void	LocationBuilder::handleParse_PathRoot(Location& location, std::string value)
{
	(void) location; (void) value;
	value = lineStripQuotes(value);
	verifyDirPath(value);
	location.SetPathRoot(value);
}

void	LocationBuilder::handleParse_IndexFiles(Location& location, std::string value)
{
	std::vector<std::string> rawFiles;
	std::vector<std::string> indexFiles;
	
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
	location.SetIndexFiles(indexFiles);
}

void	LocationBuilder::handleParse_UploadStore(Location& location, std::string value)
{
	value = lineStripQuotes(value);
	verifyUploadDirPath(value);
	location.SetPathUploadStore(value);
}

void	LocationBuilder::handleParse_AllowedMethods(Location& location, std::string value)
{
	std::set<std::string> allowedMethods;
	std::vector<std::string> methods;

	methods = splitValue(value, ',');
	if (methods.empty())
		throwLineError("allowedMethods cannot be empty");

	for (size_t i = 0; i < methods.size(); ++i)
	{
		std::string method;
		
		method = lineStripQuotes(methods[i]);
		verifyHTTPMethod(method);
		std::pair<std::set<std::string>::iterator, bool> insertResult;
		insertResult = allowedMethods.insert(method);
		if (!insertResult.second)
			throwLineError("Duplicate HTTP method: " + method);
	}
	location.SetAllowedMethods(allowedMethods);
}

void	LocationBuilder::handleParse_CGIExtension(Location& location, std::string value)
{
	std::vector<std::string> cgi;
	std::string extension;
	std::string execPath;
	
	(void)location; (void) value;
	cgi = splitValue(value, ',');
	if (cgi.size() != 2)
		throwLineError("cgiExtension requires 2 args: cgiExtension:<extension>,<exec_path>");
	extension = lineStripQuotes(cgi[0]);
	execPath = lineStripQuotes(cgi[1]);
	verifyCGIExtension(extension);
	verifyCGIExecPath(execPath);
	location.AddCGIExtension(extension, execPath);
}

void	LocationBuilder::handleParse_ClientMaxBodySize(Location& location, std::string value)
{
	(void) location; (void) value;
	location.SetClientMaxBodySize(parseAndVerifyMaxBodySize(value));
}

void	LocationBuilder::handleParse_AutoIndex(Location& location, std::string value)
{
	bool result;
	
	(void) location; (void) value;
	result = false;
	value = lineStripQuotes(value);
	if (value == "true")
		result = true;
	else if (value == "false")
		result = false;
	else
		throwLineError("Invalid value expected either \"true\" or \"false\"");
	location.SetAutoIndex(result);
}

void	LocationBuilder::handleParse_Redirect(Location& location, std::string value)
{
	std::vector<std::string> redirectRaw;
	std::string path;
	size_t code;
	
	(void) location; (void) value;
	redirectRaw = splitValue(value, ',');
	if (redirectRaw.size() != 2)
		throwLineError("Wrong value, must be:    redirect:<code>,<path>");
	code = parseAndVerifyRedirectCode(redirectRaw[0]);
	path = lineStripQuotes(redirectRaw[1]);
	verifyRedirectURL(path);
	location.SetRedirectCode(code);
	location.SetRedirectPath(path);
}