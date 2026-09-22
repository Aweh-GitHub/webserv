/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 17:19:26 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/20 02:00:01 by lupayet          ###   ########.fr       */
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
	addr.sin_addr.s_addr =  sAddr;
	fd = socket(sFamily, SOCK_STREAM, 0);
	if (!fd)
		return (std::cerr << "Error socket creation (" << sPort << ")" << std::endl, fd);
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return (std::cerr << "Error setsockopt (" << sPort << ")" << std::endl, -1);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)))
		return (std::cerr << "Error bind (" << sPort << ")" << strerror(errno) << std::endl, -2);
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
	
	const std::vector<Server> servers = this->_config.GetAllServers();

	for (std::vector<Server>::const_iterator it = servers.begin();
		 it != servers.end(); ++it)
	{
		const Server &server = *it;

		/*
		 * Don't create another socket if this IP/port
		 * is already being listened to.
		 */
		bool alreadyExists = false;
		std::cout << it->GetHostIp() << std::endl;
		std::cout << it->GetListenPort() << std::endl;

		for (std::vector<Server>::const_iterator prev = servers.begin();
			 prev != it; ++prev)
		{
			if (prev->GetHostIp() == server.GetHostIp()
				&& prev->GetListenPort() == server.GetListenPort())
			{
				alreadyExists = true;
				break;
			}
		}

		if (alreadyExists)
			continue;

		int fd = newSocket(
			AF_INET,
			server.GetListenPort(),
			inet_addr(server.GetHostIp().c_str())
		);

		if (fd < 0)
			return (0);

		AAction *action = new Socket(fd, server.GetListenPort(), server.GetHostIp());

		//addToPoll(fd);
		addToAction(action);
	}
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
