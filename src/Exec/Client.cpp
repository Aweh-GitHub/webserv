/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:52:12 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 05:50:12 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebServ.hpp"
#include <cstdlib>

const std::map<int, std::string> Client::_redirCode = Client::createRedirCode();

Client::Client(int fd, int port, std::string &ip) : 
	_port(port),
	_ip(ip),
	_serverOrigin(NULL),
	_location(NULL),
	_status(READING),
	_resReady(false)
{
	_fd = fd;
	_sBytes = 0;
	_endRequestHeader = 0;
	_bodyReceived = 0;
	_bodyLength = 0;
	_startBodyHeader = 0;
	_status = READING;
	_cgiRunning = false;
	_cgiFd = -1;
	_cgiPid = -1;
}

Client::~Client() {}

void	updatePollEvent(int fd, short events)
{
	for ( unsigned long int i = 0; i < WebServ::_pollFds.size(); i++)
	{
		if (fd == WebServ::_pollFds[i].fd)
			WebServ::_pollFds[i].events = events;
	}
}

bool	Client::setServerLocation()
{
	const std::string	requestDomain = removePort(getValue("Host", _headers));
	_serverOrigin = WebServ::_config.TryFindServer(_ip, _port, requestDomain);
	if (_serverOrigin == NULL)
		return (badRequestRes(), false);
	splitUrl(getValue("Location", _headers), _requestLocation, _requestUrlQuery);
	if (!isSafeRequestPath(_requestLocation))
	{
    	ErrorResponce(400);
    	return (false);
	}
	_location = _serverOrigin->TryFindLocation(_requestLocation);
	if (_location == NULL)
		return (badRequestRes(), false);
	return (true);
}

ssize_t	Client::maxBodyLength()
{
	std::string	contentLengthStr;
	char		*end;
	long		contentLength;
	ssize_t		maxBodySize;

	if (_location && _location->GetClientMaxBodySize() != 0)
		maxBodySize = _location->GetClientMaxBodySize();
	else
		maxBodySize = _serverOrigin->GetClientMaxBodySize();

	contentLengthStr = getValue("Content-Length", _headers);

	// No Content-Length -> no declared body size
	if (contentLengthStr.empty())
		return (maxBodySize);

	// Parse Content-Length
	end = NULL;
	contentLength = std::strtol(contentLengthStr.c_str(), &end, 10);

	// Invalid number
	if (end == contentLengthStr.c_str() || *end != '\0')
	{
		badRequestRes();
		return (-1);
	}

	// Negative Content-Length
	if (contentLength < 0)
	{
		badRequestRes();
		return (-1);
	}
	_bodyLength = contentLength;
	// Declared body is larger than configuration
	if (static_cast<size_t>(contentLength) > static_cast<size_t>(maxBodySize))
	{
		// 413 Payload Too Large, preferably
		ErrorResponce(413);
		return (-1);
	}

	return (maxBodySize);
}

void	Client::getRequest()
{
	char buffer[4096];

    ssize_t n = recv(_fd, buffer, sizeof(buffer), 0);
	
    if (n > 0)
    {
        _request.append(buffer, n);

        // Header not complete yet
        _endRequestHeader = _request.find("\r\n\r\n");
        if (_endRequestHeader == std::string::npos)
            return;
		if (_headers.empty())
			if (!parseHeader())
				return (badRequestRes());
		if (!_serverOrigin && !_location)
			if (!setServerLocation())
				return;
        _startBodyHeader = _endRequestHeader + 4;

		_maxBodyLength = maxBodyLength();

		if (_maxBodyLength < 0)
		{
            _status = WRITING;
            return;
        }

		_bodyReceived = _request.size() - _startBodyHeader;

		if (_bodyReceived > _bodyLength)
		{
			badRequestRes();
			return;
		}
        // Complete request
        if (_bodyReceived == _bodyLength)
        {
            _status = WRITING;
            return;
        }

        // Header complete, body still arriving
        _status = READING;
    }
    else if (n == 0)
    {
        WebServ::closeConnection() = _fd;
    }
}

void	Client::handleRequest()
{
	build();
	if (_cgiRunning)
		updatePollEvent(_fd, 0);
	else
		updatePollEvent(_fd, POLLOUT);
}

void	Client::sendResponce()
{
	//std::cout << _res << std::endl;
	if ( _resReady == false )
	{
		_res = _resHeader + _resBody;
		_resReady = true;
	}
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
		WebServ::_closeConnection = _fd;
	}
}

int	Client::getPort()
{
	return (_port);
}

bool	Client::internalRedirection(std::string &location)
{
	const Location	*targetLocation;
	std::string		path;
	std::string		body;

	targetLocation = _serverOrigin->TryFindLocation(location);
	if (targetLocation == NULL)
		return (false);
	if (resolvePath(targetLocation, location, _serverOrigin, path) != PATH_FILE)
		return (false);
	std::cout << "Internal redirection to: " << path << std::endl;
	if (!getFileContent(path, body))
		return (false);
	_resHeader = getHeader(404, getMimeType(path), body.size());
	_resBody = body;
	_status = SENDING;
	return (true);
}

std::map<int, std::string> Client::createRedirCode()
{
    std::map<int, std::string> m;

    m.insert(std::make_pair(301, " Moved Permanently"));
    m.insert(std::make_pair(302, " Found"));
    m.insert(std::make_pair(307, " Temporary Redirect"));

    return m;
}