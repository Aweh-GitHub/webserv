/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:05:24 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 10:11:13 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "AAction.hpp"
#include <map>

enum rStatus
{
	READING,
	WRITING,
	SENDING,
	TERMINATED
};

class Client : public AAction
{
	public:
		Client(int fd, int port);
		Client(const Client &cpy);
		Client	&operator=(const Client &other);
		~Client();
		void	action();
		int		build();
		int		getPort();
	private:
		void	getRequest();
		void	handleRequest();
		void	sendResponce();
		int			_port;
		bool	parseRequestLine(const std::string &line);
		bool	parseHeaderLine(const std::string &line);
		bool	parseHeader();
		void	badRequestRes();
		std::string	getHeader(int code, std::string type, size_t length);
		bool	handleGet(std::string &path);
		bool	handleIndex(std::string &path);
		std::map<std::string, std::string> _headers;
		rStatus		_status;
		std::string _request;
		std::string	_res;
		ssize_t		_sBytes;
		ssize_t		_bodyReceived;
		ssize_t		_bodyLength;
		static std::map<int, std::string> createRedirCode();
		static const std::map<int, std::string> _redirCode;
		std::string	redirMap(int code);
		std::string	redirect(int code);
		std::string	indexDir(std::string &path);
		Client();
};