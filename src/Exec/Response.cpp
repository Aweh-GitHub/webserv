/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:52:27 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/22 15:32:20 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebPage.hpp"
#include "Server.hpp"
#include "WebServ.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

void	Client::badRequestRes()
{
	_res = "HTTP/1.1 400 Bad Request\r\n";
	_res += "Content-Type: text/html\r\n";
	_res += "Content-Length: 50\r\n";
	_res += "Connection: close\r\n\r\n";
	_res += "<html><body><h1>400 Bad Request</h1></body></html>";
	_status = SENDING;
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

std::string	checkMethod(const std::string requestM, const std::set<std::string> &allowMethods)
{
	if (allowMethods.find(requestM) != allowMethods.end())
		return (requestM);
	else
		return ("");
}

#include <sys/stat.h>

bool	resolvePath(const Location *location,
					const std::string &requestLocation,
					const Server *server,
					std::string &endPath)
{
	std::string root = location->GetPathRoot();

	if (root.empty())
		root = server->GetPathRoot();

	// Build filesystem path
	if (!root.empty() && root[root.length() - 1] == '/' &&
		!requestLocation.empty() && requestLocation[0] == '/')
		endPath = root + requestLocation.substr(1);
	else if (!root.empty() && root[root.length() - 1] != '/' &&
			 !requestLocation.empty() && requestLocation[0] != '/')
		endPath = root + "/" + requestLocation;
	else
		endPath = root + requestLocation;

	struct stat st;

	// Path doesn't exist
	if (stat(endPath.c_str(), &st) != 0)
		return (false);

	// Regular file
	if (S_ISREG(st.st_mode))
		return (true);

	// Directory
	if (S_ISDIR(st.st_mode))
	{
		std::string directory = endPath;

		if (directory[directory.length() - 1] != '/')
			directory += '/';

		// Use location index files
		const std::vector<std::string> *indexFiles = &location->GetIndexFiles();

		// If location has no index files, use server index files
		if (indexFiles->empty())
			indexFiles = &server->GetIndexFiles();

		for (std::vector<std::string>::const_iterator it = indexFiles->begin();
			 it != indexFiles->end(); ++it)
		{
			std::string indexPath = directory + *it;

			struct stat indexSt;

			if (stat(indexPath.c_str(), &indexSt) == 0 &&
				S_ISREG(indexSt.st_mode))
			{
				endPath = indexPath;
				return (true);
			}
		}

		// Directory exists but no index file was found
		endPath = directory;
		return (false);
	}

	return (false);
}

// bool resolveUploadPath(const Location *location,
//                        const std::string &requestLocation,
//                        const Server *server,
//                        std::string &endPath)
// {
//     std::string root = location->GetPathRoot();

//     if (root.empty())
//         root = server->GetPathRoot();

//     if (!root.empty() && root[root.length() - 1] == '/' &&
//         !requestLocation.empty() && requestLocation[0] == '/')
//     {
//         endPath = root + requestLocation.substr(1);
//     }
//     else if (!root.empty() && root[root.length() - 1] != '/' &&
//              !requestLocation.empty() && requestLocation[0] != '/')
//     {
//         endPath = root + "/" + requestLocation;
//     }
//     else
//     {
//         endPath = root + requestLocation;
//     }

//     return true;
// }

void Client::build()
{
	std::cout << _request << std::endl << "boundary :" << getValue("Content-Type", _headers) << std::endl;
	if (_location->IsRedirection())
	{
		_res = redirect(_location->GetReturnCode(), _location->GetReturnPath());
		_status = SENDING;
		return ;
	}
	std::string method = checkMethod(getValue("Method", _headers) ,_location->GetAllowedMethods());
	if (method.empty())
		return (badRequestRes());
	
	//std::cout << "ENDPATH : " << endPath << std::endl;
	
	if (method == "GET" && _location->GetAllowedMethods().find(method) !=  _location->GetAllowedMethods().end())
    {
		std::string	endPath;
		if (!resolvePath(_location, _requestLocation, _serverOrigin, endPath))
		{
			if (_location->GetAutoIndex() && !handleIndex(endPath))
			{
				std::cout << endPath<< std::endl;
				return (badRequestRes());
			}
			else
				notFoundRes(404, _serverOrigin->GetErrorPages());
			_status = SENDING;
			return;
		}
        handleGet(endPath);
    }
    else if (method == "POST" && _location->GetAllowedMethods().find(method) !=  _location->GetAllowedMethods().end())
    {
    	//handlePost(endPath);
		return (badRequestRes());
    }
    else if (method == "DELETE" && _location->GetAllowedMethods().find(method) !=  _location->GetAllowedMethods().end())
    {
        return (badRequestRes());
    }
	_status = SENDING;
}