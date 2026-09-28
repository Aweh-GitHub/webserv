/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 19:52:27 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/28 19:37:44 by lupayet          ###   ########.fr       */
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

// bool	resolvePath(const Location *location,
// 					const std::string &requestLocation,
// 					const Server *server,
// 					std::string &endPath)
// {
// 	std::string root = location->GetPathRoot();

// 	if (root.empty())
// 		root = server->GetPathRoot();

// 	// Build filesystem path
// 	if (!root.empty() && root[root.length() - 1] == '/' &&
// 		!requestLocation.empty() && requestLocation[0] == '/')
// 		endPath = root + requestLocation.substr(1);
// 	else if (!root.empty() && root[root.length() - 1] != '/' &&
// 			 !requestLocation.empty() && requestLocation[0] != '/')
// 		endPath = root + "/" + requestLocation;
// 	else
// 		endPath = root + requestLocation;

// 	struct stat st;

// 	// Path doesn't exist
// 	if (stat(endPath.c_str(), &st) != 0)
// 		return (false);

// 	// Regular file
// 	if (S_ISREG(st.st_mode))
// 		return (true);

// 	// Directory
// 	if (S_ISDIR(st.st_mode))
// 	{
// 		std::string directory = endPath;

// 		if (directory[directory.length() - 1] != '/')
// 			directory += '/';

// 		// Use location index files
// 		const std::vector<std::string> *indexFiles = &location->GetIndexFiles();

// 		// If location has no index files, use server index files
// 		if (indexFiles->empty())
// 			indexFiles = &server->GetIndexFiles();

// 		for (std::vector<std::string>::const_iterator it = indexFiles->begin();
// 			 it != indexFiles->end(); ++it)
// 		{
// 			std::string indexPath = directory + *it;

// 			struct stat indexSt;

// 			if (stat(indexPath.c_str(), &indexSt) == 0 &&
// 				S_ISREG(indexSt.st_mode))
// 			{
// 				endPath = indexPath;
// 				return (true);
// 			}
// 		}

// 		// Directory exists but no index file was found
// 		endPath = directory;
// 		return (false);
// 	}

// 	return (false);
// }

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

// void Client::build()
// {
// 	//std::cout << _request << std::endl << "boundary :" << getValue("Content-Type", _headers) << std::endl;
// 	if (_location->IsRedirection())
// 	{
// 		_res = redirect(_location->GetReturnCode(), _location->GetReturnPath());
// 		_status = SENDING;
// 		return ;
// 	}
// 	std::string method = checkMethod(getValue("Method", _headers) ,_location->GetAllowedMethods());
// 	if (method.empty())
// 		return (badRequestRes());
	
// 	//std::cout << "ENDPATH : " << endPath << std::endl;
	
// 	if (method == "GET" && _location->GetAllowedMethods().find(method) !=  _location->GetAllowedMethods().end())
//     {
// 		std::string	endPath;
// 		if (!resolvePath(_location, _requestLocation, _serverOrigin, endPath))
// 		{
// 			if (_location->GetAutoIndex() && !handleIndex(endPath))
// 			{
// 				std::cout << endPath<< std::endl;
// 				return (badRequestRes());
// 			}
// 			else
// 				notFoundRes(404, _serverOrigin->GetErrorPages());
// 			_status = SENDING;
// 			return;
// 		}
//         handleGet(endPath);
//     }
//     else if (method == "POST" && _location->GetAllowedMethods().find(method) !=  _location->GetAllowedMethods().end())
//     {
//     	//handlePost(endPath);
// 		return (badRequestRes());
//     }
//     else if (method == "DELETE" && _location->GetAllowedMethods().find(method) !=  _location->GetAllowedMethods().end())
//     {
//         return (badRequestRes());
//     }
// 	_status = SENDING;
// }

// bool Client::isCGI(const std::string &path) const
// {
// 	std::string::size_type dot = path.rfind('.');

// 	if (dot == std::string::npos)
// 		return (false);

// 	std::string extension = path.substr(dot);

// 	const std::map<std::string, std::string> &cgi =
// 		_location->GetCGIExtensions();

// 	return (cgi.find(extension) != cgi.end());
// }

#include <sys/stat.h>

PathType Client::resolvePath(const Location *location,
							 const std::string &requestLocation,
							 const Server *server,
							 std::string &path)
{
	std::string	root;
	std::string	relativeLocation;
	struct stat	st;

	/*
	 * Location root overrides server root.
	 */
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

	/*
	 * Build filesystem path.
	 *
	 * Example:
	 *
	 * root            = "./www"
	 * requestLocation = "/index.html"
	 *
	 * path             = "./www/index.html"
	 */
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

	/*
	 * Check filesystem.
	 */
	std::cout << "Checking path: " << path << " - " << stat(path.c_str(), &st) << std::endl;
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

	/*
	 * Make sure directory ends with '/'.
	 */
	if (!directory.empty() &&
		directory[directory.length() - 1] != '/')
	{
		directory += '/';
	}

	/*
	 * Location index files have priority.
	 */
	const std::vector<std::string> *indexFiles =
		&_location->GetIndexFiles();

	/*
	 * If location has no index configuration,
	 * use server index files.
	 */
	if (indexFiles->empty())
		indexFiles = &_serverOrigin->GetIndexFiles();

	/*
	 * Try index files in configured order.
	 *
	 * Example:
	 *
	 * indexFiles:
	 *   index.html
	 *   index.htm
	 *   index.php
	 *
	 * First existing regular file wins.
	 */
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

	/*
	 * No index found.
	 *
	 * Keep path as the directory.
	 */
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

bool Client::resolveCGIIndex(const std::string &requestLocation,
							 std::string &scriptPath,
							 std::string &pathInfo)
{
	std::string	root;
	std::string	cgiIndex;
	struct stat	st;

	/*
	 * Get configured front controller.
	 *
	 * Example:
	 *
	 * cgiIndex = "index.php"
	 */
	cgiIndex = "index.php"; //_location->GetCGIIndex();

	if (cgiIndex.empty())
		return (false);

	/*
	 * Location root overrides server root.
	 */
	root = _location->GetPathRoot();

	if (root.empty())
		root = _serverOrigin->GetPathRoot();

	if (root.empty())
		return (false);

	/*
	 * Make sure root ends with '/'.
	 */
	if (root[root.length() - 1] != '/')
		root += '/';

	/*
	 * Build CGI script path.
	 *
	 * Example:
	 *
	 * root     = ./www/wordpress/
	 * cgiIndex = index.php
	 *
	 * scriptPath = ./www/wordpress/index.php
	 */
	scriptPath = root + cgiIndex;

	/*
	 * Verify that the CGI index actually exists.
	 */
	if (stat(scriptPath.c_str(), &st) != 0)
		return (false);

	if (!S_ISREG(st.st_mode))
		return (false);

	/*
	 * Keep the original requested URI as PATH_INFO.
	 *
	 * /hello-world
	 * /products/123
	 * /wp-admin/foo
	 */
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

	/*
	 * --------------------------------------------------
	 * 1. REDIRECTION
	 * --------------------------------------------------
	 */
	if (_location->IsRedirection())
	{
		_res = redirect(_location->GetReturnCode(),
						_location->GetReturnPath());

		_status = SENDING;
		return ;
	}

	/*
	 * --------------------------------------------------
	 * 2. CHECK METHOD
	 * --------------------------------------------------
	 */
	method = getValue("Method", _headers);
	std::cout << "Request method: " << method << std::endl;

	if (_location->GetAllowedMethods().find(method)
		== _location->GetAllowedMethods().end())
	{
		/*
		 * TODO:
		 * This should eventually be a 405 response.
		 */
		badRequestRes();
		_status = SENDING;
		return ;
	}
	/*
	 * --------------------------------------------------
	 * 3. RESOLVE URI -> FILESYSTEM
	 * --------------------------------------------------
	 *
	 * Examples:
	 *
	 * /index.php
	 *      -> ./www/wordpress/index.php
	 *
	 * /wp-admin/
	 *      -> ./www/wordpress/wp-admin/
	 *
	 * /hello-world
	 *      -> ./www/wordpress/hello-world
	 *      -> doesn't exist
	 */
	pathType = resolvePath(
		_location,
		_requestLocation,
		_serverOrigin,
		path
	);
	std::cout << "Resolved path: " << path << std::endl;
	/*
	 * --------------------------------------------------
	 * 4. EXISTING REGULAR FILE
	 * --------------------------------------------------
	 */
	if (pathType == PATH_FILE)
	{
		/*
		 * Example:
		 *
		 * /index.php
		 * /wp-login.php
		 * /test.py
		 */
		if (isCGI(path))
		{
			std::cout << "CGI script found: " << path << std::endl;
			/*
			 * Direct CGI request.
			 *
			 * /index.php
			 *
			 * scriptPath = ./www/wordpress/index.php
			 * scriptName = /index.php
			 * pathInfo   = ""
			 */
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

		/*
		 * Normal static file.
		 *
		 * GET /style.css
		 */
		if (method == "GET")
		{
			if (!handleGet(path))
				_status = SENDING;
			else
				_status = SENDING;

			return ;
		}

		/*
		 * POST to a non-CGI file.
		 *
		 * Whether this should be allowed depends on
		 * your server's semantics.
		 */
		if (method == "POST")
		{
			handlePost(path);
			return ;
		}

		/*
		 * DELETE
		 *
		 * Add handleDelete(path) when implemented.
		 */
		if (method == "DELETE")
		{
			handleDelete(path);
			return ;
		}

		badRequestRes();
		_status = SENDING;
		return ;
	}

	/*
	 * --------------------------------------------------
	 * 5. EXISTING DIRECTORY
	 * --------------------------------------------------
	 *
	 * Example:
	 *
	 * /             -> ./www/wordpress/
	 * /wp-admin/    -> ./www/wordpress/wp-admin/
	 */
	if (pathType == PATH_DIRECTORY)
	{
		/*
		 * Try index files:
		 *
		 * index.html
		 * index.php
		 * etc.
		 */
		if (resolveIndex(path))
		{
			/*
			 * resolveIndex() changed path:
			 *
			 * ./www/wordpress/
			 *       ↓
			 * ./www/wordpress/index.php
			 */

			if (isCGI(path))
			{
				/*
				 * Directory index is CGI.
				 *
				 * Request:
				 *     GET /
				 *
				 * Script:
				 *     ./www/wordpress/index.php
				 *
				 * SCRIPT_NAME:
				 *     /index.php
				 *
				 * PATH_INFO:
				 *     ""
				 */
				scriptPath = path;

				/*
				 * For an index request, SCRIPT_NAME should
				 * be the URI of the actual index script.
				 *
				 * Example:
				 *
				 * /             -> /index.php
				 * /wp-admin/    -> /wp-admin/index.php
				 */
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

			/*
			 * Static index file.
			 *
			 * Example:
			 *
			 * / -> index.html
			 */
			std::cout << path << std::endl;
			if (method == "GET")
			{
				handleGet(path);
				_status = SENDING;
				return ;
			}

			if (method == "POST")
			{
				handlePost(path);
				// _status = SENDING;
				return ;
				//return (badRequestRes());
			}

			if (method == "DELETE")
			{
				handleDelete(path);
				// _status = SENDING;
				return ;
				//return (badRequestRes());
			}
		}

		/*
		 * No index file.
		 *
		 * Try autoindex.
		 */
		if (_location->GetAutoIndex())
		{
			if (handleIndex(path))
			{
				_status = SENDING;
				return ;
			}
		}

		/*
		 * Directory exists but no index and no autoindex.
		 */
		notFoundRes(
			404,
			_serverOrigin->GetErrorPages()
		);

		_status = SENDING;
		return ;
	}

	/*
	 * --------------------------------------------------
	 * 6. PATH DOES NOT EXIST
	 * --------------------------------------------------
	 *
	 * This is the WordPress front-controller case.
	 *
	 * Example:
	 *
	 * GET /hello-world
	 *
	 * Filesystem:
	 *
	 * ./www/wordpress/hello-world
	 *                 ^ doesn't exist
	 *
	 * But:
	 *
	 * cgiIndex = index.php
	 *
	 * Therefore:
	 *
	 * ./www/wordpress/index.php
	 * PATH_INFO=/hello-world
	 */
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
			/*
			 * Make sure the configured CGI index
			 * actually has a CGI handler.
			 *
			 * Example:
			 *
			 * index.php
			 *     ↓
			 * .php
			 *     ↓
			 * /usr/bin/php-cgi
			 */
			if (isCGI(scriptPath))
			{
				/*
				 * Front controller.
				 *
				 * /hello-world
				 *
				 * becomes internally:
				 *
				 * scriptPath = ./www/wordpress/index.php
				 * scriptName = /index.php
				 * pathInfo   = /hello-world
				 */
				scriptName = "/" + std::string("index.php");

				executeCGI(
					scriptPath,
					scriptName,
					pathInfo
				);

				return ;
			}
		}

		/*
		 * No front controller.
		 */
		notFoundRes(
			404,
			_serverOrigin->GetErrorPages()
		);

		_status = SENDING;
		return ;
	}

	/*
	 * --------------------------------------------------
	 * 7. FALLBACK
	 * --------------------------------------------------
	 */
	badRequestRes();
	_status = SENDING;
}