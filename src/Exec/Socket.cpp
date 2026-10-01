/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:12:14 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 03:47:45 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebServ.hpp"
#include "Client.hpp"

Socket::Socket(int fd, size_t port, const std::string &ip) : _port(port), _ip(ip)
{
	_fd = fd;
	_cgiFd = -1;
}

Socket::~Socket() {}

void	Socket::action()
{
	int	cfd;
	Client	*r;

	cfd = accept(_fd, NULL, NULL);
	if (cfd == -1)
	{
		std::cerr << "Error Accept" << std::endl;
		return ;
	}
	fcntl(cfd, F_SETFL, O_NONBLOCK);
	r = new Client(cfd, _port, _ip);
	WebServ::addToPoll(cfd);
	WebServ::addToAction(r);
}

size_t Socket::getPort()
{
	return (_port);
}

std::string &Socket::getIp()
{
	return (_ip);
}