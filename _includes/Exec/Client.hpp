/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:05:24 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/25 01:35:07 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core_webserv.hpp"
#include "AAction.hpp"
#include "Server.hpp"
#include <map>

enum rStatus
{
	READING,
	WRITING,
	SENDING,
	TERMINATED
};

enum PathType
{
	PATH_NOT_FOUND,
	PATH_FILE,
	PATH_DIRECTORY
};

class Client : public AAction
{
	public:
		Client(int fd, int port, std::string &ip);
		Client(const Client &cpy);
		Client	&operator=(const Client &other);
		~Client();
		void	action();
		void	build();
		int		getPort();
	private:
		void	getRequest();
		void	handleRequest();
		void	sendResponce();
		int			_port;
		std::string	_ip;
		bool	parseRequestLine(const std::string &line);
		bool	parseHeaderLine(const std::string &line);
		bool	parseHeader();
		void	badRequestRes();
		void	notFoundRes(int errCode, std::map<size_t, std::string> errorPages);
		std::string	getHeader(int code, std::string type, size_t length);
		bool	handleGet(std::string &path);
		void	handlePost(const std::string &endPath);
		bool	handleIndex(std::string &path);
		//
		bool	isCGI(const std::string &path) const;
		PathType	resolvePath(const Location *location, 
			const std::string &requestLocation,
			const Server *server,
			std::string &path);
		bool	resolveIndex(std::string &path);
		bool	resolveCGIIndex(const std::string &requestLocation,
							 std::string &scriptPath,
							 std::string &pathInfo);
		void	executeCGI(const std::string &scriptPath,
						const std::string &scriptName,
						const std::string &pathInfo);
		//
		bool	setServerLocation();
		ssize_t	maxBodyLength();
		std::map<std::string, std::string> _headers;
		const Server	*_serverOrigin;
		const Location	*_location;
		std::string		_requestLocation;
		std::string		_requestUrlQuery;
		rStatus			_status;
		std::string 	_request;
		std::string		_res;
		ssize_t			_sBytes;
		size_t			_endRequestHeader;
		ssize_t			_startBodyHeader;
		ssize_t			_bodyReceived;
		ssize_t			_bodyLength;
		ssize_t			_maxBodyLength;
		bool			_cgiRunning;
		static std::map<int, std::string> createRedirCode();
		static const std::map<int, std::string> _redirCode;
		std::string	redirMap(int code);
		std::string	redirect(int code, std::string path);
		std::string	indexDir(std::string &path);
		Client();
};