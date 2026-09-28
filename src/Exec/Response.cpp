/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:52:27 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/29 00:05:22 by lupayet          ###   ########.fr       */
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
#include <sys/stat.h>

void	Client::badRequestRes()
{
	ErrorResponce(400);
}

std::string	Client::getHeader(int code, std::string type, size_t length)
{
	std::string	header;
	std::string	reason;

	reason = "Error";
	if (code == 200)
		reason = "OK";
	else if (code == 201)
		reason = "Created";
	else if (code == 204)
		reason = "No Content";
	else if (code == 400)
		reason = "Bad Request";
	else if (code == 401)
		reason = "Unauthorized";
	else if (code == 403)
		reason = "Forbidden";
	else if (code == 404)
		reason = "Not Found";
	else if (code == 405)
		reason = "Method Not Allowed";
	else if (code == 413)
		reason = "Payload Too Large";
	else if (code == 500)
		reason = "Internal Server Error";
	else if (code == 501)
		reason = "Not Implemented";
	else if (code == 502)
		reason = "Bad Gateway";
	else if (code == 503)
		reason = "Service Unavailable";
	header += "HTTP/1.1 " + ft_itoa(code) + " " + reason + "\r\n";
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

PathType Client::resolvePath(const Location *location,
							 const std::string &requestLocation,
							 const Server *server,
							 std::string &path)
{
	std::string	root;
	std::string	relativeLocation;
	struct stat	st;

	root = location->GetPathRoot();

	if (root.empty())
		root = server->GetPathRoot();

	relativeLocation = requestLocation;
	if (location->GetName() != "/" &&
		requestLocation.compare(0, location->GetName().length(),
			location->GetName()) == 0)
	{
		relativeLocation = requestLocation.substr(location->GetName().length());
		if (relativeLocation.empty())
			relativeLocation = "/";
	}

	if (root.empty())
	{
		path = relativeLocation;
	}
	else if (root[root.length() - 1] == '/' &&
			 !relativeLocation.empty() &&
			 relativeLocation[0] == '/')
	{
		path = root + relativeLocation.substr(1);
	}
	else if (root[root.length() - 1] != '/' &&
			 !relativeLocation.empty() &&
			 relativeLocation[0] != '/')
	{
		path = root + "/" + relativeLocation;
	}
	else
	{
		path = root + relativeLocation;
	}

	if (stat(path.c_str(), &st) != 0)
	{
		return (PATH_NOT_FOUND);
	}

	if (S_ISREG(st.st_mode))
	{
		return (PATH_FILE);
	}

	if (S_ISDIR(st.st_mode))
	{
		return (PATH_DIRECTORY);
	}

	return (PATH_NOT_FOUND);
}

bool Client::resolveIndex(std::string &path)
{
	std::string	directory;
	std::string	indexPath;
	struct stat	st;

	directory = path;

	if (!directory.empty() &&
		directory[directory.length() - 1] != '/')
	{
		directory += '/';
	}

	const std::vector<std::string> *indexFiles =
		&_location->GetIndexFiles();

	if (indexFiles->empty())
		indexFiles = &_serverOrigin->GetIndexFiles();

	for (std::vector<std::string>::const_iterator it =
			indexFiles->begin();
		 it != indexFiles->end();
		 ++it)
	{
		indexPath = directory + *it;

		if (stat(indexPath.c_str(), &st) == 0 &&
			S_ISREG(st.st_mode))
		{
			path = indexPath;
			return (true);
		}
	}

	path = directory;

	return (false);
}

bool Client::isCGI(const std::string &path) const
{
	std::string::size_type	dot;
	std::string				extension;

	dot = path.rfind('.');

	if (dot == std::string::npos)
		return (false);

	extension = path.substr(dot);

	const std::map<std::string, std::string> &cgi =
		_location->GetCGIExtensions();

	if (cgi.find(extension) == cgi.end())
		return (false);

	return (true);
}

static std::string findCGIIndex(
		const std::vector<std::string> &indexFiles,
		const std::map<std::string, std::string> &cgiExtensions)
{
	std::string::size_type dot;

	for (std::vector<std::string>::const_iterator it = indexFiles.begin();
		it != indexFiles.end(); ++it)
	{
		dot = it->rfind('.');
		if (dot != std::string::npos &&
			cgiExtensions.find(it->substr(dot)) != cgiExtensions.end())
			return (*it);
	}
	return ("");
}

bool Client::resolveCGIIndex(const std::string &requestLocation,
							 std::string &scriptPath,
							 std::string &pathInfo)
{
	std::string	root;
	std::string	cgiIndex;
	const std::vector<std::string> *indexFiles;
	struct stat	st;

	indexFiles = &_location->GetIndexFiles();
	if (indexFiles->empty())
		indexFiles = &_serverOrigin->GetIndexFiles();
	cgiIndex = findCGIIndex(*indexFiles, _location->GetCGIExtensions());

	if (cgiIndex.empty())
		return (false);

	root = _location->GetPathRoot();

	if (root.empty())
		root = _serverOrigin->GetPathRoot();

	if (root.empty())
		return (false);

	if (root[root.length() - 1] != '/')
		root += '/';

	scriptPath = root + cgiIndex;

	if (stat(scriptPath.c_str(), &st) != 0)
		return (false);

	if (!S_ISREG(st.st_mode))
		return (false);

	pathInfo = requestLocation;

	return (true);
}

void Client::build()
{
	std::string	method;
	std::string	path;
	std::string	scriptPath;
	std::string	scriptName;
	std::string	pathInfo;
	PathType	pathType;

	if (_cgiRunning)
	{
		readCGIOutput();
		return ;
	}

	std::cout << "Request: " << getValue("Method", _headers) << " " << getValue("Host", _headers) << " " << _requestLocation << std::endl;

	if (_location->IsRedirection())
	{
		_res = redirect(_location->GetReturnCode(),
						_location->GetReturnPath());

		_status = SENDING;
		return ;
	}

	method = getValue("Method", _headers);

	if (_location->GetAllowedMethods().find(method)
		== _location->GetAllowedMethods().end())
	{
		ErrorResponce(405);
		return ;
	}

	pathType = resolvePath(
		_location,
		_requestLocation,
		_serverOrigin,
		path
	);

	if (pathType == PATH_FILE)
	{
		if (isCGI(path))
		{
			scriptPath = path;
			scriptName = _requestLocation;
			pathInfo.clear();

			executeCGI(
				scriptPath,
				scriptName,
				pathInfo
			);
			return ;
		}

		if (method == "GET")
		{
			if (!handleGet(path))
				_status = SENDING;
			else
				_status = SENDING;
			return ;
		}

		if (method == "POST")
		{
			handlePost(path);
			return ;
		}

		if (method == "DELETE")
		{
			handleDelete(path);
			return ;
		}

		badRequestRes();
		_status = SENDING;
		return ;
	}

	if (pathType == PATH_DIRECTORY)
	{
		if (resolveIndex(path))
		{
			if (isCGI(path))
			{
				scriptPath = path;

				if (_requestLocation.empty() ||
					_requestLocation[_requestLocation.length() - 1] != '/')
				{
					scriptName = _requestLocation;
				}
				else
				{
					std::string indexFile =
						path.substr(path.rfind('/') + 1);

					scriptName = _requestLocation + indexFile;
				}

				pathInfo.clear();

				executeCGI(
					scriptPath,
					scriptName,
					pathInfo
				);

				return ;
			}

			if (method == "GET")
			{
				handleGet(path);
				_status = SENDING;
				return ;
			}

			if (method == "POST")
			{
				handlePost(path);
				return ;
			}

			if (method == "DELETE")
			{
				handleDelete(path);
				return ;
			}
		}

		if (_location->GetAutoIndex())
		{
			if (handleIndex(path))
			{
				_status = SENDING;
				return ;
			}
		}

		ErrorResponce(404);
		return ;
	}

	if (pathType == PATH_NOT_FOUND)
	{
		if (method == "POST")
		{
			handlePost(path);
			return ;
		}

		if (method == "DELETE")
		{
			handleDelete(path);
			return ;
		}

		if (resolveCGIIndex(
				_requestLocation,
				scriptPath,
				pathInfo))
		{
			if (isCGI(scriptPath))
			{
				scriptName = "/" + std::string("index.php");

				executeCGI(
					scriptPath,
					scriptName,
					pathInfo
				);

				return ;
			}
		}

		ErrorResponce(404);
		return ;
	}

	badRequestRes();
	_status = SENDING;
}