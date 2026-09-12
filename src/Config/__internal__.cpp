/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __internal__.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:08:34 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/12 17:48:57 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "__internal__.hpp"
#include "ConfigBuilder.hpp"
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>


bool	isValueObject(std::string& value)
{
	// {
	return (value[0] == '{');
}

bool	isValueMultiArg(std::string& value)
{
	return (value.find(',') != std::string::npos);
}

bool	isSkipLine(std::string& line)
{
	return (line.empty() || isCommentLine(line));
}

bool	isCommentLine(std::string& line)
{
	const std::string whitespace = " \t\r\n";
	size_t	i;

	i = line.find_first_not_of(whitespace);
	if (i != std::string::npos && line[i] == '#' && line[i + 1] == '!')
		return (true);
	return (false);
}

bool	lineStripComment(std::string& line)
{
	size_t commentIndex;

	commentIndex = line.find("#!");
	if (commentIndex == std::string::npos)
		return (false);
	line = line.substr(0, commentIndex);
	return (true);
}

void	verifyQuotesSanity(std::string& line)
{
	size_t	firstQuote = line.find('"');
	size_t	lastQuote = line.rfind('"');

	if (firstQuote == std::string::npos)
		throwLineError("Expected quotes: " + line);
	if (firstQuote == lastQuote)
		throwLineError("Quotes must be in pair: " + line);
	for (size_t i = 0; i < firstQuote; ++i)
	{
		if (!std::isspace(line[i]))
			std::cout << "line[i]: " << line[i] << std::endl, throwLineError("Unexpected character before quote: " + line);
	}
	for (size_t i = lastQuote + 1; i < line.length(); ++i)
	{
		if (!std::isspace(line[i]))
			throwLineError("Unexpected character after quote: " + line);
	}
	for (size_t i = firstQuote + 1; i < lastQuote; ++i)
	{
		if (line[i] == '"')
			throwLineError("Unauthorized quote inside value: " + line);
	}
}

std::string	lineStripQuotes(std::string& line)
{
	size_t first, last;
	
	verifyQuotesSanity(line);
	first = line.find_first_of("\"");
	last = line.find_last_of("\"");
	if (first != std::string::npos && last != std::string::npos && first < last)
		return (line.substr(first + 1, last - first - 1));
	return (line);
}

//
// TODO: throw quotes on key
//
bool	lineParseKeyValue(std::string& line, std::string& outKey, std::string& outValue)
{
	size_t	separatorIndex;

	separatorIndex = line.find(':');
	if (separatorIndex == std::string::npos)
		return (false);
	outKey = line.substr(0, separatorIndex);
	outKey = lineTrimSpaces(outKey);
	
	outValue = line.substr(separatorIndex + 1);
	lineStripComment(outValue);
	outValue = lineTrimSpaces(outValue);
	return (true);
}

size_t toSize(const std::string& str)
{
	if (str.empty() || str[0] == '-')
		throw std::invalid_argument("Invalid numeric value (empty or negative): " + str);

	std::istringstream iss(str);
	size_t result;
	char remaining;

	if (!(iss >> result))
		throw std::invalid_argument("Conversion failed for: " + str);
	if (iss >> remaining)
		throw std::invalid_argument("Trailing characters found in: " + str);

	return (result);
}

std::vector<std::string> splitValue(const std::string& str, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	if (str.empty())
		return (tokens);

	if (str[0] == delimiter || str[str.length() - 1] == delimiter)
		throwLineError("Value list contains empty element (starts or ends with delimiter)");

	while (std::getline(ss, token, delimiter))
	{
		if (token.empty())
			throwLineError("Value list contains empty element");
		tokens.push_back(token);
	}
	return (tokens);
}

std::string	lineTrimSpaces(const std::string& str)
{
	const std::string spaces = " \t\r\n";
	size_t first, last;

	first = str.find_first_not_of(spaces);
	if (first == std::string::npos)
		return ("");
	last = str.find_last_not_of(spaces);
	return (str.substr(first, last - first + 1));
}

bool	isLineEndBracket(std::string line)
{
	if (isCommentLine(line))
		return (false);
	line = lineTrimSpaces(line);
	return (line[0] == '}');
}

void verifyIP(const std::string& ip)
{
	if (ip.empty())
		throwLineError("Expected hostIP to not be empty IP value");
	if (ip[ip.length() - 1] == '.')
		throwLineError("Expected hostIP value to not end with '.'");

	std::istringstream iss(ip);
	std::string chunk;
	int chunkCount;
	int value;

	chunkCount = 0;
	while (std::getline(iss, chunk, '.'))
	{
		chunkCount++;
		
		if (chunkCount > 4 || chunk.empty() || chunk.length() > 3)
		{
			std::cout << "cCount: " << chunkCount << " | chunkEmpty: " << chunk.empty() << " | chunkLength: " << chunk.length() << " | value: " << chunk << std::endl;
			throwLineError("Invalid hostIP value");
		}

		for (size_t i = 0; i < chunk.length(); ++i) {
			if (!std::isdigit(chunk[i]))
				throwLineError("Expected hostIP value to be only numbers (0 - 255)");
		}

		std::istringstream(chunk) >> value;
		if (value < 0 || value > 255)
			throwLineError("Expected hostIP value to be only numbers (0 - 255)");
	}

	if (chunkCount != 4)
		throwLineError("Expected hostIP value chunks to be exactly 4");
}

size_t parseAndVerifyPort(const std::string& value)
{
	std::istringstream iss(value);
	size_t port;
	
	if (value.empty())
		throwLineError("Port value cannot be empty.");
	for (size_t i = 0; i < value.length(); ++i)
	{
		if (!std::isdigit(value[i]))
			throwLineError("Invalid port format: '" + value + "' contains non-digit characters.");
	}
	iss >> port;
	if (iss.fail())
		throwLineError("Port conversion failed: value is too large.");
	if (port == 0 || port > 65535)
		throwLineError("Port out of range (must be 1-65535): " + value);
	return (port);
}

void verifyDomain(const std::string& domain)
{
	if (domain.empty() || domain.length() > 253)
		throwLineError("Domain length is too long or too short. (1 - 253)");
	if (domain[0] == '.' || domain[0] == '-' || domain[domain.length() - 1] == '.' || domain[domain.length() - 1] == '-')
		throwLineError("Starting or ending with invalid character ");
	for (size_t i = 0; i < domain.length(); ++i)
	{
		char c;
		
		c = domain[i];
		if (!std::isalnum(c) && c != '.' && c != '-')
			throwLineError("Invalid character '" + toString(c) + "'");
		if (c == '.' && i > 0 && domain[i - 1] == '.')
			throwLineError("Invalid character '" + toString(c) + "'");
	}
}

size_t parseAndVerifyRedirectCode(const std::string& value)
{
	std::istringstream iss(value);
	size_t code;

	if (value.empty())
		throwLineError("Redirect code cannot be empty");
	for (size_t i = 0; i < value.length(); ++i)
	{
		if (!std::isdigit(value[i]))
			throwLineError("Invalid format: '" + value + "' must contains digit only");
	}
	iss >> code;
	if (iss.fail())
		throwLineError("Invalid value, too large");
	if (code != 301 && code != 302 && code != 303 && code != 307 && code != 308)
		throwLineError("Invalid value (must be: 301-302-303-307-308)");
	return (code);
}

void	throwLineError(const std::string& str)
{
	throw std::runtime_error("Error: at line " + toString(ConfigBuilder::LineIndex) + ": " + str);
}

//
// for: pathRoot
//
void verifyDirPath(const std::string& path)
{
	struct stat info;

	if (path.empty())
		throwLineError("Directory path cannot be empty");
	if (path.find("..") != std::string::npos)
		throwLineError("Invalid '..' in path: " + path);
	if (stat(path.c_str(), &info) != 0)
		throwLineError("Directory does not exist: " + path);
	if (!S_ISDIR(info.st_mode))
		throwLineError("Path is not a Directory: " + path);
	if (access(path.c_str(), R_OK | X_OK) != 0)
		throwLineError("Invalid permissions: " + path);
}

//
// for: pathUploadStore
//
void verifyUploadDirPath(const std::string& path)
{
	verifyDirPath(path);
	if (access(path.c_str(), W_OK | X_OK) != 0)
		throwLineError("Missing write/execute permissions for upload directory: " + path);
}

//
// for: name, redirect
//
void verifyURL(const std::string& path)
{
	if (path.empty())
		throwLineError("URL path cannot be empty");
	if (path[0] != '/')
		throwLineError("URL must start with '/': " + path);
	if (path.find("..") != std::string::npos)
		throwLineError("Invalid '..' in path: " + path);

	for (size_t i = 0; i < path.length(); ++i)
	{
		char c = path[i];
		if (!std::isalnum(c) && c != '/' && c != '-' && c != '_' && c != '.')
			throwLineError("Invalid character '" + toString(c) + "' in URL: " + path);
	}
}

void verifyRedirectURL(const std::string& path)
{
	if (path.empty())
		throwLineError("Redirect URL cannot be empty");
	if (path[0] != '/' && path.find("http://") != 0 && path.find("https://") != 0)
		throwLineError("Invalid redirect URL (must start with: '/', 'http://', 'https://'): " + path);
	if (path.find("..") != std::string::npos)
		throwLineError("Invalid '..' in path: " + path);

	for (size_t i = 0; i < path.length(); ++i)
	{
		char c = path[i];
		// On inclut ':', '?', '&' et '=' pour supporter les URLs absolues et les paramètres
		if (!std::isalnum(c) && c != '/' && c != '-' && c != '_' && c != '.' && c != ':' && c != '?' && c != '&' && c != '=')
			throwLineError("Invalid character '" + toString(c) + "' in redirect URL: " + path);
	}
}

void verifyIndexFileName(const std::string& fileName)
{
	if (fileName.empty())
		throwLineError("IndexFile name cannot be empty");
	if (fileName.find('/') != std::string::npos)
		throwLineError("IndexFile name invalid '/': " + fileName);
	if (fileName.find("..") != std::string::npos)
		throwLineError("IndexFile invalid '..': " + fileName);
	for (size_t i = 0; i < fileName.length(); ++i)
	{
		if (std::isspace(fileName[i]) || !std::isprint(fileName[i]))
			throwLineError("Invalid character in index file name: " + fileName);
	}
}

size_t	parseAndVerifyMaxBodySize(const std::string& value)
{
	std::istringstream iss(value);
	size_t size;
	
	if (value.empty())
		throwLineError("clientMaxBodySize cannot be empty");
	for (size_t i = 0; i < value.length(); ++i)
	{
		if (!std::isdigit(value[i]))
			throwLineError("Invalid clientMaxBodySize must be digit only: " + value);
	}
	iss >> size;
	if (iss.fail())
		throwLineError("Invalid value, too large");
	return (size);
}

size_t parseAndVerifyErrorCode(const std::string& value)
{
	std::istringstream iss(value);
	size_t code;

	if (value.empty())
		throwLineError("Error code cannot be empty");
	for (size_t i = 0; i < value.length(); ++i)
	{
		if (!std::isdigit(value[i]))
			throwLineError("Invalid error code must be digit only: " + value);
	}
	iss >> code;
	if (iss.fail())
		throwLineError("Invalid error code value, too large");
	if (code < 400 || code > 599)
		throwLineError("Invalid value (must be 400-599)");
	return (code);
}

void	verifyHTTPMethod(const std::string& method)
{
	if (method != "GET" && method != "POST" && method != "DELETE")
		throwLineError("Invalid method: '" + method + "'. (Must be: GET, POST, DELETE)");
}

void verifyCGIExtension(const std::string& extension)
{
	if (extension.empty() || extension[0] != '.' || extension.length() < 2)
		throwLineError("Invalid CGI format (must follow: '.<ext>'): " + extension);
	if (extension.find('/') != std::string::npos)
		throwLineError("Invalid CGI extension '/': " + extension);
	for (size_t i = 0; i < extension.length(); ++i)
	{
		if (std::isspace(extension[i]) || !std::isprint(extension[i]))
			throwLineError("Invalid CGI extension: " + extension);
	}
}

void verifyCGIExecPath(const std::string& path)
{
	struct stat info;

	if (path.empty())
		throwLineError("CGI exec path cannot be empty");
	if (path.find("..") != std::string::npos)
		throwLineError("Invalid '..' CGI exec path: " + path);
	if (stat(path.c_str(), &info) != 0)
		throwLineError("CGI exec does not exist: " + path);
	if (!S_ISREG(info.st_mode))
		throwLineError("CGI exec path is not a file: " + path);
	if (access(path.c_str(), X_OK) != 0)
		throwLineError("Missing exec permission: " + path);
}