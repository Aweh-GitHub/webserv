/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 16:22:21 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/02 06:03:13 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "AAction.hpp"
#include <vector>

/*enum	Action
{
	SOCKET,
	CLIENT
};*/

struct ListeningSocket
{
	int fd;
	sockaddr_in addr;
};

class Server
{
	public:
		Server();
		~Server();
		int		newSocket(sa_family_t sFamily, in_port_t sPort, in_addr_t sAddr);
		//AAction	*newAction(int fd, Action type);
		static int		addToPoll(int fd);
		static void		addToAction(AAction *a);
		static int		&closeConnection();
		static void		quit();
		int	init();
		//void	run();
		//void	stop();
		//void	reload();
		static std::vector<pollfd> _pollFds;
		static std::vector<AAction*> _A;
		static int		_closeConnection;
		static bool				_isRunning;
	private:
		Server(const Server &cpy);
		Server	&operator=(const Server &other);
};
