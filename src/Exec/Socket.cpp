/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 01:12:14 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/20 02:01:10 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "WebServ.hpp"
#include "Client.hpp"

Socket::Socket(int fd, size_t port, const std::string &ip) : _port(port), _ip(ip)
{
	_fd = fd;
	#if DEBUG
	std::cout << "Socket constructor called" << std::endl;
	#endif
}

Socket::Socket(const Socket &cpy)
{
	_fd = cpy._fd;
	#if DEBUG
	std::cout << "Socket copy constructor called" << std::endl;
	#endif
}

Socket	&Socket::operator=(const Socket &other)
{
	if (this != &other)
		_fd = other._fd;
	#if DEBUG
	std::cout << "Socket copy assignement operator called" << std::endl;
	#endif
	return (*this);
}

Socket::~Socket()
{
	#if DEBUG
	std::cout << "Socket destructor called" << std::endl;
	#endif
}

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