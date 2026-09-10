/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:19:26 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/10 14:54:49 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Socket.hpp"
#include "Client.hpp"

std::vector<pollfd> WebServ::_pollFds;
std::vector<AAction*> WebServ::_A;
int	WebServ::_closeConnection = 0;
bool WebServ::_isRunning = true;
Config	WebServ::_config;

int	WebServ::newSocket(sa_family_t sFamily, in_port_t sPort, in_addr_t sAddr)
{
	int			fd;
	int			opt = 1;
	sockaddr_in	addr;

	addr.sin_family = sFamily;
	addr.sin_port = htons(sPort);
	addr.sin_addr.s_addr =  htonl(sAddr);
	fd = socket(sFamily, SOCK_STREAM, 0);
	if (!fd)
		return (std::cerr << "Error socket creation (" << sPort << ")" << std::endl, fd);
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return (std::cerr << "Error setsockopt (" << sPort << ")" << std::endl, -1);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
		return (std::cerr << "Error bind (" << sPort << ")" << std::endl, -2);
	if (listen(fd, SOMAXCONN))
		return (std::cerr << "Error listen (" << sPort << ")" << std::endl, 1);
	addToPoll(fd);
	return (std::cout << "New socket (" << sPort << ")" << "created" << std::endl, fd);
}
/*
AAction	*WebServ::newAction(int fd, Action type)
{
	AAction *action = NULL;

	if (type == SOCKET)
		action = new Socket(fd);
	else
		action = new Request(fd);
	return (action);
}*/

int	WebServ::addToPoll(int fd)
{
	pollfd	p;
	p.fd = fd;
	p.events = POLLIN;
	p.revents = 0;
	_pollFds.push_back(p);
	return (fd);
}

void	WebServ::addToAction(AAction *a)
{
	_A.push_back(a);
}

int	&WebServ::closeConnection()
{
	return (_closeConnection);
}

int	WebServ::init()
{
	if (!this->_config.GetIsInitialized())
		return (0);
	int	socket80 = newSocket(AF_INET, 80, INADDR_ANY);
	if (!socket80)
		return (0);
	AAction	*action80 = new Socket(socket80, 80);
	addToPoll(socket80);
	addToAction(action80);
	return (1);
}

void	WebServ::quit()
{
	for (long unsigned int i = 0; i < _A.size(); i++)
		delete _A[i];
	_A.clear();
	_isRunning = false;
}

WebServ::WebServ() {}

WebServ::~WebServ()
{
	#if DEBUG
	std::cout << "WebServ destructor called" << std::endl;
	#endif
}
