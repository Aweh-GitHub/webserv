/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responce.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:52:27 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 07:38:32 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Responce.hpp"
#include "Client.hpp"
#include "WebPage.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

std::string load_file(const std::string &path) {
	std::ifstream file(path.c_str());
	std::ostringstream content;
	content << file.rdbuf(); // Read file into content
	return content.str(); // Return the string
}

int	getFileContent(std::string &filename, std::string &out)
{
	std::string	tmp;
	tmp = load_file(filename);
	if (!tmp.size())
		return 0;
	out += tmp;
	return 1;
}

void	Client::badRequestRes()
{
	_res += "HTTP/1.1 400 Bad Request\r\n";
	_res += "Content-Type: text/html\r\n";
	_res += "Content-Length: 50\r\n";
	_res += "Connection: close\r\n\r\n";
	_res += "<html><body><h1>400 Bad Request</h1></body></html>";
}

std::string	ft_itoa(int n)
{
	std::ostringstream oss;
	oss << n;
	return (oss.str());
}

std::string	getHeader(int code, std::string type, size_t length)
{
	std::string	header;
	header += "HTTP/1.1 " + ft_itoa(code) + " OK\r\n";
	header += "Content-Type: " + type + "\r\n";
	header += "Content-Length: " + ft_itoa(length) + "\r\n";
	header += "Connection: close\r\n\r\n";
	return (header);
}

int	getMethod(std::string &responce)
{
	std::string	path = "/home/lupayet/42-Cursus/WebServ/html/index.html";
	std::string	body;
	if (!getFileContent(path, body))
		return (501);
	responce += getHeader(200, "text/html", body.size());
	responce += body;
	return (1);
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

std::string	Client::redirect(int code)
{
	
	std::string	redirection;

	redirection += "HTTP/1.1 ";
	redirection += redirMap(code);
	redirection += "\r\n";
	redirection += "Location: https://www.youtube.com/watch?v=dQw4w9WgXcQ\r\n";
	redirection += "Content-Length: 0\r\n\r\n";
	return (redirection);
}

std::string	Client::indexDir(std::string path)
{
	WebPage	p(path);
	std::ostringstream body;

	body << "<ul>";
	DIR *dir = opendir(path.c_str());
	struct dirent	*ent = readdir(dir);
	while (ent != NULL)
	{
		body << "<li><a href=\"" << ent->d_name << "\">" << ent->d_name << "</li>";
		ent = readdir(dir);
	};
	body << "</ul>";
	p.addToBody(body.str());
	return (p.str());
}

bool	Client::handleGet()
{
	
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
	std::string	body = indexDir("./html/");
	_res += getHeader(200, "text/html", body.size());
	_res += body;
	std::cout << _res << std::endl;
	return 0;
}