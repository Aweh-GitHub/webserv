/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 14:57:48 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/29 00:59:50 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <cstdio>
#include <errno.h>
#include <fstream>

static bool getMultipartBoundary(const std::string &contentType,
		std::string &boundary)
{
	std::string::size_type boundaryStart;

	if (contentType.find("multipart/form-data") == std::string::npos)
		return (false);
	boundaryStart = contentType.find("boundary=");
	if (boundaryStart == std::string::npos)
		return (false);
	boundaryStart += 9;
	boundary = contentType.substr(boundaryStart);
	if (!boundary.empty() && boundary[0] == '"')
	{
		if (boundary.length() < 2 || boundary[boundary.length() - 1] != '"')
			return (false);
		boundary = boundary.substr(1, boundary.length() - 2);
	}
	return (!boundary.empty());
}

static std::string getUploadFileName(const std::string &requestLocation)
{
	std::string::size_type slash;

	slash = requestLocation.rfind('/');
	if (slash == std::string::npos || slash + 1 == requestLocation.length())
		return ("");
	return (requestLocation.substr(slash + 1));
}

static std::string getMultipartFileName(const std::string &partHeaders)
{
	std::string::size_type filenameStart;
	std::string::size_type filenameEnd;

	filenameStart = partHeaders.find("filename=\"");
	if (filenameStart == std::string::npos)
		return ("");
	filenameStart += 10;
	filenameEnd = partHeaders.find('"', filenameStart);
	if (filenameEnd == std::string::npos)
		return ("");
	return (partHeaders.substr(filenameStart, filenameEnd - filenameStart));
}

static bool writeMultipartFiles(const std::string &body,
		const std::string &boundary, const std::string &uploadStore,
		const std::string &fallbackPath)
{
	std::string		marker;
	std::string		partHeaders;
	std::string		fileName;
	std::string		filePath;
	std::string::size_type markerPosition;
	std::string::size_type partStart;
	std::string::size_type headersEnd;
	std::string::size_type contentEnd;
	std::string::size_type slash;
	std::ofstream	file;
	size_t			fileCount;

	marker = "--" + boundary;
	markerPosition = body.find(marker);
	fileCount = 0;
	while (markerPosition != std::string::npos)
	{
		partStart = markerPosition + marker.length();
		if (body.compare(partStart, 2, "--") == 0)
			break;
		if (body.compare(partStart, 2, "\r\n") != 0)
			return (false);
		partStart += 2;
		headersEnd = body.find("\r\n\r\n", partStart);
		if (headersEnd == std::string::npos)
			return (false);

		partHeaders = body.substr(partStart, headersEnd - partStart);
		fileName = getMultipartFileName(partHeaders);
		contentEnd = body.find("\r\n" + marker, headersEnd + 4);
		if (contentEnd == std::string::npos)
			return (false);

		if (!fileName.empty())
		{
			slash = fileName.find_last_of("/\\");
			if (slash != std::string::npos)
				fileName = fileName.substr(slash + 1);
			if (fileName.empty())
				return (false);

			filePath = uploadStore;
			if (!filePath.empty())
			{
				if (filePath[filePath.length() - 1] != '/')
					filePath += '/';
				filePath += fileName;
			}
			else
				filePath = fallbackPath;

			file.open(filePath.c_str(), std::ios::binary | std::ios::trunc);
			if (!file.is_open())
				return (false);
			file.write(body.data() + headersEnd + 4,
				static_cast<std::streamsize>(contentEnd - headersEnd - 4));
			file.close();
			if (file.fail())
				return (false);
			++fileCount;
		}

		markerPosition = body.find(marker, contentEnd + 2);
	}
	return (fileCount != 0);
}

void	Client::handlePost(const std::string &endPath)
{
	std::string	filePath;
	std::string	fileName;
	std::string	contentType;
	std::string	boundary;
	std::string	body;
	std::ofstream	file;
	size_t		bodyStart;

	filePath = endPath;
	if (!_location->GetPathUploadStore().empty())
	{
		fileName = getUploadFileName(_requestLocation);
		if (fileName.empty())
			return (badRequestRes());
		filePath = _location->GetPathUploadStore();
		if (filePath[filePath.length() - 1] != '/')
			filePath += '/';
		filePath += fileName;
	}

	bodyStart = _startBodyHeader;
	if (bodyStart > _request.length() ||
		_request.length() - bodyStart < static_cast<size_t>(_bodyLength))
		return (badRequestRes());
	body = _request.substr(bodyStart, _bodyLength);
	contentType = getValue("Content-Type", _headers);
	if (getMultipartBoundary(contentType, boundary))
	{
		if (!writeMultipartFiles(body, boundary,
			_location->GetPathUploadStore(), filePath))
			return (badRequestRes());
		_res = "HTTP/1.1 201 Created\r\n";
		_res += "Content-Length: 0\r\n";
		_res += "Connection: close\r\n\r\n";
		_status = SENDING;
		return ;
	}

	file.open(filePath.c_str(), std::ios::binary | std::ios::trunc);
	if (!file.is_open())
		return (badRequestRes());
	if (_bodyLength > 0)
		file.write(_request.data() + bodyStart, _bodyLength);
	file.close();
	if (file.fail())
		return (badRequestRes());

	_res = "HTTP/1.1 201 Created\r\n";
	_res += "Content-Length: 0\r\n";
	_res += "Connection: close\r\n\r\n";
	_status = SENDING;
}

void	Client::handleDelete(const std::string &endPath)
{
	std::string	filePath;
	std::string	fileName;

	filePath = endPath;
	if (!_location->GetPathUploadStore().empty())
	{
		fileName = getUploadFileName(_requestLocation);
		if (fileName.empty())
			return (badRequestRes());
		filePath = _location->GetPathUploadStore();
		if (filePath[filePath.length() - 1] != '/')
			filePath += '/';
		filePath += fileName;
	}
	std::cout << "Deleting file: " << filePath << std::endl;
	if (std::remove(filePath.c_str()) != 0)
	{
		if (errno == ENOENT)
		{
			_res = "HTTP/1.1 404 Not Found\r\n";
			_res += "Content-Length: 0\r\n";
			_res += "Connection: close\r\n\r\n";
			_status = SENDING;
			return ;
		}
		return (badRequestRes());
	}

	_res = "HTTP/1.1 204 No Content\r\n";
	_res += "Content-Length: 0\r\n";
	_res += "Connection: close\r\n\r\n";
	_status = SENDING;
}