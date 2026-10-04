/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 01:04:09 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/03 05:33:45 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebServ.hpp"
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
	//std::string				body;

	headerEnd = cgiOutput.find("\r\n\r\n");

	if (headerEnd != std::string::npos)
	{
		headers = cgiOutput.substr(0, headerEnd);
		_resBody = cgiOutput.substr(headerEnd + 4);
	}
	else
	{
		headerEnd = cgiOutput.find("\n\n");

		if (headerEnd == std::string::npos)
			return (false);

		headers = cgiOutput.substr(0, headerEnd);
		_resBody = cgiOutput.substr(headerEnd + 2);
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
			_resHeader += name + ": " + value + "\r\n";
		}
	}

	std::ostringstream response;

	response << "HTTP/1.1 "
			 << statusCode
			 << " "
			 << statusText
			 << "\r\n";

	response << _resHeader;

	response << "Content-Length: "
			 << _resBody.length()
			 << "\r\n";

	response << "\r\n";

	_resHeader = response.str();

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
		ErrorResponce(500);
		return ;
	}

	extension = scriptPath.substr(dot);

	const std::map<std::string, std::string> &cgi =
		_location->GetCGIExtensions();

	std::map<std::string, std::string>::const_iterator it =
		cgi.find(extension);

	if (it == cgi.end())
	{
		ErrorResponce(500);
		return ;
	}

	executable = it->second;

	if (pipe(inPipe) == -1)
	{
		ErrorResponce(500);
		return ;
	}

	if (pipe(outPipe) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		ErrorResponce(500);
		return ;
	}

	pid = fork();
	if (pid == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);

		ErrorResponce(500);
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
		if (!pathInfo.empty())
			envStrings.push_back("PATH_INFO=" + pathInfo);
		else
			envStrings.push_back("PATH_INFO=/");
		//envStrings.push_back("PATH_INFO=" + pathInfo);
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
		for (std::map<std::string, std::string>::const_iterator header =
				_headers.begin(); header != _headers.end(); ++header)
		{
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
			std::cerr << "Setting env: " << envStrings[i] << std::endl;
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
	if (fcntl(inPipe[1], F_SETFL, O_NONBLOCK) == -1 ||
		fcntl(outPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		close(inPipe[1]);
		close(outPipe[0]);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		ErrorResponce(500);
		return ;
	}

	_cgiOutput.clear();
	_cgiPid = pid;
	_cgiRunning = true;
	_cgiFd = outPipe[0];
	WebServ::addToPoll(_cgiFd);
	if ((_headers["Method"] == "POST" ||
		_headers["Method"] == "PUT") && _bodyLength > 0)
	{
		_cgiInputFd = inPipe[1];
		_cgiInputOffset = _startBodyHeader;
		WebServ::addToPoll(_cgiInputFd);
		updatePollEvent(_cgiInputFd, POLLOUT);
	}
	else
	{
		close(inPipe[1]);
	}
}

void Client::writeCGIInput()
{
	ssize_t n;

	if (_cgiInputFd == -1)
		return ;
	n = write(_cgiInputFd, _request.data() + _cgiInputOffset,
		_request.size() - _cgiInputOffset);
	if (n > 0)
	{
		_cgiInputOffset += static_cast<size_t>(n);
		if (_cgiInputOffset == _request.size())
		{
			close(_cgiInputFd);
			_cgiInputFd = -1;
		}
	}
	else if (n < 0)
	{
		close(_cgiInputFd);
		_cgiInputFd = -1;
		kill(_cgiPid, SIGKILL);
		waitpid(_cgiPid, NULL, 0);
		_cgiPid = -1;
		_cgiRunning = false;
		ErrorResponce(500);
		updatePollEvent(_fd, POLLOUT);
	}
}

void Client::readCGIOutput()
{
	char	buffer[4096];
	ssize_t	n;
	int		status;

	if (_cgiFd != -1)
	{
		n = read(_cgiFd, buffer, sizeof(buffer));
		#ifndef DEBUG
		std::cout << "Read " << n << " bytes from CGI output." << std::endl;
		#endif
		if (n > 0)
			_cgiOutput.append(buffer, n);
		else if (n == 0)
		{
			close(_cgiFd);
			_cgiFd = -1;
			if (!parseCGIResponse(_cgiOutput))
			{
				waitpid(_cgiPid, NULL, 0);
				_cgiPid = -1;
				_cgiRunning = false;
				ErrorResponce(502);
				updatePollEvent(_fd, POLLOUT);
				return ;
			}
			waitpid(_cgiPid, &status, 0);
			_cgiPid = -1;
			_cgiRunning = false;
			_status = SENDING;
			updatePollEvent(_fd, POLLOUT);
			return ;
		}
		else
		{
			close(_cgiFd);
			_cgiFd = -1;
			waitpid(_cgiPid, NULL, 0);
			_cgiPid = -1;
			_cgiRunning = false;
			ErrorResponce(500);
			updatePollEvent(_fd, POLLOUT);
			return ;
		}
	}
}