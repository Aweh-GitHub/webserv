/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responce.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:52:27 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 10:22:34 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebPage.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

void	Client::badRequestRes()
{
	_res += "HTTP/1.1 400 Bad Request\r\n";
	_res += "Content-Type: text/html\r\n";
	_res += "Content-Length: 50\r\n";
	_res += "Connection: close\r\n\r\n";
	_res += "<html><body><h1>400 Bad Request</h1></body></html>";
}

std::string	Client::getHeader(int code, std::string type, size_t length)
{
	std::string	header;
	header += "HTTP/1.1 " + ft_itoa(code) + " OK\r\n";
	header += "Content-Type: " + type + "\r\n";
	header += "Content-Length: " + ft_itoa(length) + "\r\n";
	header += "Connection: close\r\n\r\n";
	return (header);
}

std::string	Client::redirMap(int code)
{
	std::string	str;
	std::map<int, std::string>::const_iterator it = _redirCode.find(code);
	if (it != _redirCode.end())
		str = ft_itoa(it->first) + it->second;
	else
		str = "302 Found";
	return (str);
}

int	Client::build()
{
	if (_headers.empty())
		if (!parseHeader())
			return (badRequestRes(), 0);
	std::map<std::string, std::string>::iterator it;
	std::cout << "==== REQUEST HEADER ====" << std::endl;
	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		std::cout << it->first << " = " << it->second << std::endl;
	}
	//if (request.getPort() != )
	//getMethod(responce);
	//redirect(302)

	std::string	location = getValue("Location", _headers);
	std::string	endPath = std::string("./html") + location;
	if (location[location.length() - 1] == '/')
	{
		std::cout << "HERE" << std::endl;
		handleIndex(endPath);
	}
	else
		handleGet(endPath);

	//handleIndex();
	_status = SENDING;
	return 0;
}