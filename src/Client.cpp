/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:52:12 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 08:46:12 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include <cstdlib>

const std::map<int, std::string> Client::_redirCode = Client::createRedirCode();

Client::Client(int fd, int port) : _port(port), _status(READING)
{
	_fd = fd;
	_sBytes = 0;
	_bodyReceived = 0;
	_bodyLength = 0;
	_status = READING;
	#if DEBUG
	std::cout << "Client constructor called " << _fd << std::endl;
	#endif
}

Client::Client(const Client &cpy) : _status(cpy._status), _res(cpy._res) 
{
	_fd = cpy._fd;
	#if DEBUG
	std::cout << "Client copy constructor called" << std::endl;
	#endif
}

Client	&Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_res = other._res;
		_status = other._status;
	}
	#if DEBUG
	std::cout << "Client copy assignement operator called" << std::endl;
	#endif
	return (*this);
}

Client::~Client()
{
	#if DEBUG
	std::cout << "Client destructor called" << std::endl;
	#endif
}

static void	updatePoll(int fd)
{
	for ( unsigned long int i = 0; i < Server::_pollFds.size(); i++)
	{
		if (fd == Server::_pollFds[i].fd)
			Server::_pollFds[i].events = POLLOUT;
	}
}

void	Client::getRequest()
{
	char	buffer[4096];
	ssize_t n = recv(_fd, buffer, sizeof(buffer), 0);
	if (n > 0)
	{
		_request.append(buffer, n);
		ssize_t	headerEnd = _request.find("\r\n\r\n");
		if (static_cast<std::string::size_type>(headerEnd) == std::string::npos)
		{
			_status = WRITING;
			return ;
		}
		size_t bodyStart = headerEnd + 4;
		size_t cPos = _request.find("Content-Length: ");
		if (cPos != std::string::npos)
		{
			std::cout << "in if cPos" << std::endl;
			size_t cEnd = _request.find_first_of("\r\n", cPos);
			std::string contentLength = _request.substr(cPos + 16, cEnd - (cPos + 16));
			_bodyLength = std::atol(contentLength.c_str());
			_bodyReceived = _request.size() - bodyStart;
		}
		if (_bodyReceived >= _bodyLength)
		{
			_status = WRITING;
			return ;
		}
	}
	else if (n == 0)
	{
		close(_fd);
		Server::closeConnection() = _fd;
	}
	/*
	else
	{
	//max try before closing the connnection
	}
	*/
}

void	Client::handleRequest()
{
	//_res = _request;
	build();
	//std::cout << _res << std::endl;
	updatePoll(_fd);
}

void	Client::sendResponce()
{
	//std::cout << _res << std::endl;
	ssize_t n = send(_fd, _res.c_str() + _sBytes, _res.size() - _sBytes, 0);
	if (n > 0)
	{
		_sBytes += n;
		if (_sBytes == static_cast<ssize_t>(_res.size()))
			_status = TERMINATED;
	}
}

void	Client::action()
{
	if (_status == READING)
		getRequest();
	if (_status == WRITING)
		handleRequest();
	else if (_status == SENDING)
		sendResponce();
	else if (_status == TERMINATED)
	{
		close(_fd);
		Server::_closeConnection = _fd;
	}
}

int	Client::getPort()
{
	return (_port);
}

std::map<int, std::string> Client::createRedirCode()
{
    std::map<int, std::string> m;

    m.insert(std::make_pair(301, " Moved Permanently"));
    m.insert(std::make_pair(302, " Found"));
    m.insert(std::make_pair(307, " Temporary Redirect"));

    return m;
}