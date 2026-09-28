/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 01:04:09 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/25 01:52:54 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>

void Client::executeCGI(const std::string &scriptPath,
						const std::string &scriptName,
						const std::string &pathInfo)
{
	int		inPipe[2];
	int		outPipe[2];
	pid_t	pid;
	int		status;

	std::string::size_type	dot;
	std::string				extension;
	std::string				executable;

	/*
	 * --------------------------------------------------
	 * Find CGI executable
	 * --------------------------------------------------
	 */
	dot = scriptPath.rfind('.');

	if (dot == std::string::npos)
	{
		badRequestRes();
		return ;
	}

	extension = scriptPath.substr(dot);

	const std::map<std::string, std::string> &cgi =
		_location->GetCGIExtensions();

	std::map<std::string, std::string>::const_iterator it =
		cgi.find(extension);

	if (it == cgi.end())
	{
		badRequestRes();
		return ;
	}

	executable = it->second;

	/*
	 * --------------------------------------------------
	 * Create pipes
	 * --------------------------------------------------
	 *
	 * inPipe:
	 *
	 * parent -> child stdin
	 *
	 * outPipe:
	 *
	 * child stdout -> parent
	 */
	if (pipe(inPipe) == -1)
	{
		badRequestRes();
		return ;
	}

	if (pipe(outPipe) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		badRequestRes();
		return ;
	}

	/*
	 * --------------------------------------------------
	 * Fork
	 * --------------------------------------------------
	 */
	pid = fork();

	if (pid == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);

		badRequestRes();
		return ;
	}

	/*
	 * --------------------------------------------------
	 * CHILD
	 * --------------------------------------------------
	 */
	if (pid == 0)
	{
		/*
		 * stdin <- parent
		 */
		dup2(inPipe[0], STDIN_FILENO);

		/*
		 * stdout -> parent
		 */
		dup2(outPipe[1], STDOUT_FILENO);

		/*
		 * Close duplicated descriptors.
		 */
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);

		/*
		 * CGI environment
		 */

		setenv("GATEWAY_INTERFACE",
			   "CGI/1.1",
			   1);

		setenv("REQUEST_METHOD",
			   _headers["Method"].c_str(),
			   1);

		setenv("SCRIPT_FILENAME",
			   scriptPath.c_str(),
			   1);

		setenv("SCRIPT_NAME",
			   scriptName.c_str(),
			   1);

		setenv("PATH_INFO",
			   pathInfo.c_str(),
			   1);

		setenv("REQUEST_URI",
			   _requestLocation.c_str(),
			   1);

		/*
		 * Query string
		 */
		setenv("QUERY_STRING",
			   _requestUrlQuery.c_str(),
			   1);

		/*
		 * Content information.
		 */
		if (_headers.find("Content-Type") != _headers.end())
		{
			setenv("CONTENT_TYPE",
				   _headers["Content-Type"].c_str(),
				   1);
		}

		if (_headers.find("Content-Length") != _headers.end())
		{
			setenv("CONTENT_LENGTH",
				   _headers["Content-Length"].c_str(),
				   1);
		}

		/*
		 * Server information.
		 */
		setenv("SERVER_PROTOCOL",
			   "HTTP/1.1",
			   1);

		setenv("SERVER_PORT",
			   ft_itoa(_port).c_str(),
			   1);

		setenv("REMOTE_ADDR",
			   _ip.c_str(),
			   1);

		setenv("REDIRECT_STATUS", "1", 1);

		/*
		 * Host.
		 */
		if (_headers.find("Host") != _headers.end())
		{
			setenv("HTTP_HOST",
				   _headers["Host"].c_str(),
				   1);
		}

		/*
		 * Build argv.
		 *
		 * PHP:
		 *
		 * argv[0] = /usr/bin/php-cgi
		 * argv[1] = ./www/wordpress/index.php
		 */
		char *argv[3];

		argv[0] = const_cast<char *>(executable.c_str());
		argv[1] = const_cast<char *>(scriptPath.c_str());
		argv[2] = NULL;

		execve(executable.c_str(),
			   argv,
			   environ);

		/*
		 * execve failed.
		 */
		exit(1);
	}

	/*
	 * --------------------------------------------------
	 * PARENT
	 * --------------------------------------------------
	 */

	close(inPipe[0]);
	close(outPipe[1]);

	/*
	 * POST/PUT body -> CGI stdin.
	 *
	 * _request should contain the complete HTTP request
	 * according to your current parser.
	 *
	 * Ideally you should have a separate _body string.
	 */
	if (_headers["Method"] == "POST" ||
		_headers["Method"] == "PUT")
	{
		if (_bodyLength > 0)
		{
			/*
			 * You should ideally write exactly the body,
			 * not the complete HTTP request.
			 */
			/*
			 * write(inPipe[1], body, bodyLength);
			 */
		}
	}

	/*
	 * No more CGI input.
	 */
	close(inPipe[1]);

	/*
	 * --------------------------------------------------
	 * Read CGI output
	 * --------------------------------------------------
	 */
	_res.clear();

	char	buffer[4096];
	ssize_t	n;

	while ((n = read(outPipe[0], buffer, sizeof(buffer))) > 0)
	{
		_res.append(buffer, n);
	}

	close(outPipe[0]);

	/*
	 * Wait for CGI process.
	 */
	waitpid(pid, &status, 0);

	_cgiRunning = false;

	/*
	 * CGI output already contains headers such as:
	 *
	 * Content-Type: text/html
	 *
	 * and:
	 *
	 * Status: 200 OK
	 *
	 * You need a parser here to convert CGI headers
	 * into your HTTP response.
	 */

	_status = SENDING;
}