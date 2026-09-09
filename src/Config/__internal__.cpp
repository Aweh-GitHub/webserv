/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __internal__.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:08:34 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/04 15:07:20 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "__internal__.hpp"
#include <iostream>
#include <vector>

bool	isValueObject(std::string& value)
{
	// {
	return (value[0] == '{');
}

bool	isValueMultiArg(std::string& value)
{
	return (value.find(',') != std::string::npos);
}

bool	isCommentLine(std::string& line)
{
	const std::string whitespace = " \t\r\n";
	size_t	i;

	i = line.find_first_not_of(whitespace);
	if (line[i] && line[i] == '/' && line[i + 1] && line[i + 1] == '/')
		return (true);
	return (false);
}

bool	lineStripComment(std::string& line)
{
	size_t commentIndex;

	commentIndex = line.find("//");
	if (commentIndex == std::string::npos)
		return (false);
	line = line.substr(0, commentIndex);
	return (true);
}

std::string	lineStripQuotes(std::string& line)
{
	size_t first, last;
	
	first = line.find_first_of("\"'");
	last = line.find_last_of("\"'");
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
	outValue = lineTrimSpaces(outValue);
	lineStripComment(outValue);
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

std::vector<std::string> split(const std::string& str, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		if (!token.empty())
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
	line = lineStripQuotes(line);
	return (line[0] == '}');
}
