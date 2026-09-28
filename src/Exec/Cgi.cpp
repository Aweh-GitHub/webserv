/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 01:04:09 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/28 23:55:17 by lupayet          ###   ########.fr       */
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
#include <sstream>

bool Client::parseCGIResponse(const std::string &cgiOutput)
{
	std::string::size_type	headerEnd;
	std::string				headers;
	std::string				body;

	headerEnd = cgiOutput.find("\r\n\r\n");

	if (headerEnd != std::string::npos)
	{
		headers = cgiOutput.substr(0, headerEnd);
		body = cgiOutput.substr(headerEnd + 4);
	}
	else
	{
		headerEnd = cgiOutput.find("\n\n");

		if (headerEnd == std::string::npos)
			return (false);

		headers = cgiOutput.substr(0, headerEnd);
		body = cgiOutput.substr(headerEnd + 2);
	}

	int statusCode = 200;
	std::string statusText = "OK";

	std::istringstream stream(headers);
	std::string line;

	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);

		std::string::size_type colon = line.find(':');

		if (colon == std::string::npos)
			continue;

		std::string name = line.substr(0, colon);
		std::string value = line.substr(colon + 1);

		while (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		if (name == "Status")
		{
			std::istringstream statusStream(value);

			statusStream >> statusCode;

			std::getline(statusStream, statusText);

			while (!statusText.empty() && statusText[0] == ' ')
				statusText.erase(0, 1);
		}
		else
		{
			_res += name + ": " + value + "\r\n";
		}
	}

	std::ostringstream response;

	response << "HTTP/1.1 "
			 << statusCode
			 << " "
			 << statusText
			 << "\r\n";

	response << _res;

	response << "Content-Length: "
			 << body.length()
			 << "\r\n";

	response << "\r\n";

	response << body;

	_res = response.str();

	return (true);
}

void Client::executeCGI(const std::string &scriptPath,
						const std::string &scriptName,
						const std::string &pathInfo)
{
	int		inPipe[2];
	int		outPipe[2];
	pid_t	pid;

	std::string::size_type	dot;
	std::string				extension;
	std::string				executable;

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

	if (pid == 0)
	{
		dup2(inPipe[0], STDIN_FILENO);
		dup2(outPipe[1], STDOUT_FILENO);

		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);

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

		setenv("QUERY_STRING",
			   _requestUrlQuery.c_str(),
			   1);

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

		setenv("SERVER_PROTOCOL",
			   "HTTP/1.1",
			   1);

		setenv("SERVER_PORT",
			   ft_itoa(_port).c_str(),
			   1);

		setenv("SERVER_NAME",
       			removePort(_headers["Host"]).c_str(),
       			1);

		setenv("REMOTE_ADDR",
			   _ip.c_str(),
			   1);

		setenv("REDIRECT_STATUS", "1", 1);

		if (_headers.find("Host") != _headers.end())
		{
			setenv("HTTP_HOST",
				   _headers["Host"].c_str(),
				   1);
		}
		
		setenv("HTTP_COOKIE", _headers["Cookie"].c_str(), 1);

		char *argv[3];

		argv[0] = const_cast<char *>(executable.c_str());
		argv[1] = const_cast<char *>(scriptPath.c_str());
		argv[2] = NULL;

		execve(executable.c_str(), argv, environ);

		exit(1);
	}

	close(inPipe[0]);
	close(outPipe[1]);

	if (_headers["Method"] == "POST" ||
		_headers["Method"] == "PUT")
	{
		if (_bodyLength > 0)
		{
			size_t total = _startBodyHeader;

   			while (total < _request.size())
    		{
        		ssize_t n = write(inPipe[1],
             	       	_request.data() + total,
                        _request.size() - total);

        		if (n <= 0)
        		{
        		    break;
        		}
        		total += static_cast<size_t>(n);
			}
		}
	}

	close(inPipe[1]);

	if (fcntl(outPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(outPipe[0]);
		waitpid(pid, NULL, 0);
		badRequestRes();
		return ;
	}

	_res.clear();
	_cgiOutput.clear();
	_cgiFd = outPipe[0];
	_cgiPid = pid;
	_cgiRunning = true;
}

void Client::readCGIOutput()
{
	char	buffer[4096];
	ssize_t	n;
	int		status;
	pid_t	waitResult;

	if (_cgiFd != -1)
	{
		while (true)
		{
			n = read(_cgiFd, buffer, sizeof(buffer));
			if (n > 0)
			{
				_cgiOutput.append(buffer, n);
				continue;
			}
			if (n == 0)
			{
				close(_cgiFd);
				_cgiFd = -1;
				if (!parseCGIResponse(_cgiOutput))
				{
					_cgiRunning = false;
					badRequestRes();
					return ;
				}
				break;
			}
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;

			close(_cgiFd);
			_cgiFd = -1;
			_cgiRunning = false;
			badRequestRes();
			return ;
		}
	}

	waitResult = waitpid(_cgiPid, &status, WNOHANG);
	if (waitResult == _cgiPid)
	{
		_cgiPid = -1;
		_cgiRunning = false;
		_status = SENDING;
	}
	else if (waitResult == -1)
	{
		_cgiPid = -1;
		_cgiRunning = false;
		badRequestRes();
	}
}