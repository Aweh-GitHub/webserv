/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:52:12 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/11 20:44:29 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebServ.hpp"
#include <cctype>
#include <cstdlib>
#include <limits>

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
	_maxBodyLength = 0;
	_chunkedBody = false;
	_chunkedState = 0;
	_chunkedParseOffset = 0;
	_chunkedChunkSize = 0;
	_startBodyHeader = 0;
	_status = READING;
	_cgiRunning = false;
	_cgiInputFd = -1;
	_cgiInputOffset = 0;
	_cgiFd = -1;
	_cgiPid = -1;
	_cgiPollTicks = 0;
	_idlePollTicks = 0;
}

Client::~Client()
{
	cleanupCGI(true);
}

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
		return (ErrorResponce(400), false);
	splitUrl(getValue("Location", _headers), _requestLocation, _requestUrlQuery);
	if (!isSafeRequestPath(_requestLocation))
	{
    	ErrorResponce(400);
    	return (false);
	}
	_location = _serverOrigin->TryFindLocation(_requestLocation);
	if (_location == NULL)
		return (ErrorResponce(400), false);
	return (true);
}

bool	Client::maxBodyLength()
{
	std::map<std::string, std::string>::const_iterator contentLengthHeader;
	std::map<std::string, std::string>::const_iterator transferEncodingHeader;
	std::string	contentLengthStr;
	size_t		contentLength;
	size_t		first;
	size_t		last;

	if (_location && _location->GetMaxBodySizeSet())
		_maxBodyLength = _location->GetClientMaxBodySize();
	else
		_maxBodyLength = _serverOrigin->GetClientMaxBodySize();
	transferEncodingHeader = _headers.find("Transfer-Encoding");
	if (transferEncodingHeader != _headers.end() &&
		transferEncodingHeader->second.find("chunked") != std::string::npos)
	{
		_chunkedBody = true;
		return (true);
	}

	contentLengthHeader = _headers.find("Content-Length");
	if (contentLengthHeader == _headers.end())
		return (true);
	contentLengthStr = contentLengthHeader->second;
	first = 0;
	last = contentLengthStr.length();
	while (last > first && (contentLengthStr[last - 1] == ' ' ||
		contentLengthStr[last - 1] == '\t'))
		--last;
	if (first == last)
	{
		ErrorResponce(400);
		return (false);
	}
	contentLengthStr = contentLengthStr.substr(first, last - first);

	contentLength = 0;
	for (size_t i = 0; i < contentLengthStr.length(); ++i)
	{
		unsigned char digit;

		if (!std::isdigit(static_cast<unsigned char>(contentLengthStr[i])))
		{
			ErrorResponce(400);
			return (false);
		}
		digit = static_cast<unsigned char>(contentLengthStr[i] - '0');
		if (contentLength >
			(std::numeric_limits<size_t>::max() - digit) / 10)
		{
			ErrorResponce(413);
			return (false);
		}
		contentLength = contentLength * 10 + digit;
	}

	// Declared body is larger than configuration
	if (contentLength > _maxBodyLength)
	{
		// 413 Payload Too Large, preferably
		ErrorResponce(413);
		return (false);
	}

	_bodyLength = contentLength;

	return (true);
}

int	Client::decodeChunkedBody()
{
	while (_chunkedState != 4)
	{
		if (_chunkedState == 0)
		{
			std::string::size_type lineEnd = _request.find("\r\n",
				_chunkedParseOffset);
			if (lineEnd == std::string::npos)
				return (0);
			std::string sizeLine = _request.substr(_chunkedParseOffset,
				lineEnd - _chunkedParseOffset);
			std::string::size_type extension = sizeLine.find(';');
			if (extension != std::string::npos)
				sizeLine.erase(extension);
			if (sizeLine.empty())
				return (-1);
			_chunkedChunkSize = 0;
			for (size_t i = 0; i < sizeLine.length(); ++i)
			{
				size_t digit;
				if (sizeLine[i] >= '0' && sizeLine[i] <= '9')
					digit = static_cast<size_t>(sizeLine[i] - '0');
				else if (sizeLine[i] >= 'a' && sizeLine[i] <= 'f')
					digit = static_cast<size_t>(sizeLine[i] - 'a' + 10);
				else if (sizeLine[i] >= 'A' && sizeLine[i] <= 'F')
					digit = static_cast<size_t>(sizeLine[i] - 'A' + 10);
				else
					return (-1);
				if (_chunkedChunkSize >
					(std::numeric_limits<size_t>::max() - digit) / 16)
					return (3);
				_chunkedChunkSize = _chunkedChunkSize * 16 + digit;
			}
			_chunkedParseOffset = lineEnd + 2;
			if (_chunkedChunkSize > _maxBodyLength)
				return (3);
			_chunkedState = (_chunkedChunkSize == 0) ? 3 : 1;
		}
		else if (_chunkedState == 1)
		{
			size_t available = _request.size() - _chunkedParseOffset;
			size_t toCopy = std::min(available, _chunkedChunkSize);
			_chunkedDecodedBody.append(_request, _chunkedParseOffset, toCopy);
			_chunkedParseOffset += toCopy;
			_chunkedChunkSize -= toCopy;
			if (_chunkedDecodedBody.size() > _maxBodyLength)
				return (3);
			if (_chunkedChunkSize != 0)
				return (0);
			_chunkedState = 2;
		}
		else if (_chunkedState == 2)
		{
			if (_request.size() - _chunkedParseOffset < 2)
				return (0);
			if (_request.compare(_chunkedParseOffset, 2, "\r\n") != 0)
				return (-1);
			_chunkedParseOffset += 2;
			_chunkedState = 0;
		}
		else
		{
			if (_request.compare(_chunkedParseOffset, 2, "\r\n") == 0)
			{
				_chunkedParseOffset += 2;
				_chunkedState = 4;
			}
			else
			{
				std::string::size_type trailersEnd = _request.find(
					"\r\n\r\n", _chunkedParseOffset);
				if (trailersEnd == std::string::npos)
					return (0);
				_chunkedParseOffset = trailersEnd + 4;
				_chunkedState = 4;
			}
		}
	}
	return (1);
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
				return (ErrorResponce(400));
		if (!_serverOrigin && !_location)
			if (!setServerLocation())
				return;
        _startBodyHeader = _endRequestHeader + 4;

		if (!maxBodyLength())
            return;
		if (_chunkedBody)
		{
			if (_chunkedState == 0 && _chunkedParseOffset == 0)
				_chunkedParseOffset = _startBodyHeader;
			int chunkStatus = decodeChunkedBody();
			if (chunkStatus == 3)
				return (ErrorResponce(413));
			if (chunkStatus < 0)
				return (ErrorResponce(400));
			if (chunkStatus == 0)
				return;
			_request.erase(_startBodyHeader);
			_request.append(_chunkedDecodedBody);
			_bodyReceived = _chunkedDecodedBody.size();
			_bodyLength = _bodyReceived;
			_status = WRITING;
			updatePollEvent(_fd, POLLOUT);
			return;
		}

		_bodyReceived = _request.size() - _startBodyHeader;
		if (_bodyReceived > _bodyLength)
		{
			ErrorResponce(413);
			return;
		}
        // Complete request
        if (_bodyReceived == _bodyLength)
        {
            _status = WRITING;
			updatePollEvent(_fd, POLLOUT);
            return;
        }

        // Header complete, body still arriving
        _status = READING;
    }
	else
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
	if ( _resReady == false )
	{
		_res = _resHeader + _resBody;
		_resReady = true;
	}
	ssize_t n = send(_fd, _res.c_str() + _sBytes, _res.size() - _sBytes,
		MSG_NOSIGNAL);
	if (n > 0)
	{
		_sBytes += n;
		if (_sBytes == static_cast<ssize_t>(_res.size()))
			_status = TERMINATED;
	}
	else if (n == 0)
		return ;
	else
		WebServ::closeConnection() = _fd;
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
		// std::cout << "Connection terminated for client: "<< std::endl;
		// std::cout << _request << std::endl;
		// std::cout << "Response sent: " << std::endl;
		// std::cout << _res << std::endl;
		WebServ::_closeConnection = _fd;
	}
}

void	Client::action(int fd, short revents)
{
	if (fd == _fd)
		_idlePollTicks = 0;
	if (fd == _cgiInputFd)
		writeCGIInput();
	else if (fd == _cgiFd)
		readCGIOutput();
	else
		action();
	(void)revents;
}

int	Client::getCgiInputFd()
{
	return (_cgiInputFd);
}

int	Client::getPort()
{
	return (_port);
}

bool	Client::internalRedirection(std::string &location, int error)
{
	const Location	*targetLocation;
	std::string		path;
	std::string		body;

	targetLocation = _serverOrigin->TryFindLocation(location);
	if (targetLocation == NULL)
		return (false);
	if (resolvePath(targetLocation, location, _serverOrigin, path) != PATH_FILE)
	{
		std::cout << path << std::endl;
		return (false);
	}
	std::cout << "Internal redirection to: " << path << std::endl;
	getFileContent(path, body);
	_resHeader = getHeader(error, getMimeType(path), body.size());
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
