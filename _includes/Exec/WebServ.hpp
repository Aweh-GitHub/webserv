/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 16:22:21 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/02 06:03:13 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core_webserv.hpp"
#include "AAction.hpp"
#include "Config.hpp"
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

class WebServ
{
	public:
		WebServ();
		~WebServ();
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
		static int				_closeConnection;
		static bool				_isRunning;
		static Config			_config;
	private:
		WebServ(const WebServ &cpy);
		WebServ	&operator=(const WebServ &other);
};