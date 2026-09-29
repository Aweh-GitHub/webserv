/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 01:04:09 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/29 04:24:57 by lupayet          ###   ########.fr       */
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
#include <cctype>
#include <iostream>
#include <sstream>

static std::string makeCGIHeaderName(const std::string &headerName)
{
	std::string name = "HTTP_";

	for (size_t i = 0; i < headerName.length(); ++i)
	{
		if (headerName[i] == '-')
			name += '_';
		else
			name += static_cast<char>(std::toupper(
				static_cast<unsigned char>(headerName[i])));
	}
	return (name);
}

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

		std::vector<std::string> envStrings;
		std::vector<char *> env;
		std::string envName;

		envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		envStrings.push_back("REQUEST_METHOD=" + _headers["Method"]);
		envStrings.push_back("SCRIPT_FILENAME=" + scriptPath);
		envStrings.push_back("SCRIPT_NAME=" + scriptName);
		envStrings.push_back("PATH_INFO=" + pathInfo);
		envStrings.push_back("REQUEST_URI=" + _requestLocation);
		envStrings.push_back("QUERY_STRING=" + _requestUrlQuery);
		if (_headers.find("Content-Type") != _headers.end())
			envStrings.push_back("CONTENT_TYPE=" + _headers["Content-Type"]);
		if (_headers.find("Content-Length") != _headers.end())
			envStrings.push_back("CONTENT_LENGTH=" + _headers["Content-Length"]);
		envStrings.push_back("SERVER_PROTOCOL=" + _headers["Version"]);
		envStrings.push_back("SERVER_PORT=" + ft_itoa(_port));
		envStrings.push_back("SERVER_NAME=" + removePort(_headers["Host"]));
		envStrings.push_back("REMOTE_ADDR=" + _ip);
		envStrings.push_back("REDIRECT_STATUS=1");
		std::cerr << _headers["Content-Length"] << " ?= " << _bodyLength << std::endl;
		for (std::map<std::string, std::string>::const_iterator header =
				_headers.begin(); header != _headers.end(); ++header)
		{
			//std::cerr << header->first << ": " << header->second << std::endl;
			if (header->first == "Method" ||
				header->first == "Location" ||
				header->first == "Version" ||
				header->first == "Content-Type" ||
				header->first == "Content-Length")
				continue;
			envName = makeCGIHeaderName(header->first);
			envStrings.push_back(envName + "=" + header->second);
		}
		for (size_t i = 0; i < envStrings.size(); ++i)
		{
			//std::cerr << "Env: " << envStrings[i] << std::endl;
			env.push_back(const_cast<char *>(envStrings[i].c_str()));
		}
		env.push_back(NULL);

		char *argv[3];

		argv[0] = const_cast<char *>(executable.c_str());
		argv[1] = const_cast<char *>(scriptPath.c_str());
		argv[2] = NULL;

		execve(executable.c_str(), argv, &env[0]);

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
			if (n < 0)
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