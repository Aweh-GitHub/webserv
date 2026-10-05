/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 09:28:28 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 05:57:24 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <map>
#include <string>

bool Client::parseRequestLine(const std::string &line)
{
	size_t firstSpace = line.find(' ');
	size_t secondSpace = line.find(' ', firstSpace + 1);

	if (firstSpace == std::string::npos ||
		secondSpace == std::string::npos)
		return false;

	_headers["Method"] =
		line.substr(0, firstSpace);

	_headers["Location"] =
		line.substr(
			firstSpace + 1,
			secondSpace - firstSpace - 1);

	_headers["Version"] =
		line.substr(secondSpace + 1);

	if (_headers["Method"].empty() ||
		_headers["Location"].empty() ||
		_headers["Version"].empty())
		return false;

	if (_headers["Version"] != "HTTP/1.1")
		return false;

	return true;
}

bool Client::parseHeaderLine(const std::string &line)
{
	size_t colon = line.find(':');

	if (colon == std::string::npos)
		return false;

	std::string name = line.substr(0, colon);
	std::string value = line.substr(colon + 1);

	if (name == "Method" || name == "Location" || name == "Version")
		return (true);

	// Remove leading whitespace
	size_t first = value.find_first_not_of(" \t");

	if (first != std::string::npos)
		value = value.substr(first);
	else
		value = "";

	_headers[name] = value;

	return true;
}

bool	Client::parseHeader()
{
	size_t		headerEnd = _request.find("\r\n\r\n");
	std::string headers = _request.substr(0, headerEnd);

    size_t start = 0;
    size_t pos;

    // First line = request line
    pos = headers.find("\r\n", start);

    if (pos == std::string::npos)
        return false;

    std::string requestLine = headers.substr(start, pos - start);

    if (!parseRequestLine(requestLine))
        return false;

    // Remaining lines = headers
    start = pos + 2;

    while (start < headers.size())
    {
        pos = headers.find("\r\n", start);

        std::string line;

        if (pos == std::string::npos)
            line = headers.substr(start);
        else
            line = headers.substr(start, pos - start);

        if (!line.empty())
        {
            if (!parseHeaderLine(line))
                return false;
        }

        if (pos == std::string::npos)
            break;

        start = pos + 2;
    }

    // HTTP/1.1 requires Host
    if (_headers["Version"] == "HTTP/1.1" &&
        _headers.find("Host") == _headers.end())
    {
        return false;
    }
    return true;
}