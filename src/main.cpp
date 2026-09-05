/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 00:36:24 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/05 14:46:55 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "WebServ.hpp"
#include "Colors.hpp"
#include "Config.hpp"

void handleSignal(int sig)
{
    if (sig == SIGINT)
	{
		WebServ::quit();
	}
}

int	main(int ac, char **av)
{
	WebServ	srv;

	if (ac != 2)
		return (std::cerr << RED << "Error: requires ./program <config_file>" << RST << std::endl, 1);
	std::string pathConfigFile(av[1]);
	
	try
	{
		WebServ::_config = new Config(pathConfigFile);
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RST << '\n';
	}
	signal(SIGINT, handleSignal);
	if (!srv.init())
		return 1;
	while (srv._isRunning)
	{
		poll(srv._pollFds.data(), srv._pollFds.size(), -1);
		for (size_t i = 0; i < srv._pollFds.size(); i++)
		{
			if (srv._pollFds[i].revents & POLLIN)
			{
				for (size_t j = 0; j < srv._A.size(); j++)
				{
					if (srv._pollFds[i].fd == srv._A[j]->getFd())
					{
						srv._A[j]->action();
						if (srv.closeConnection() == srv._pollFds[i].fd)
						{
							delete srv._A[j];
							srv._pollFds.erase(srv._pollFds.begin() + i);
							srv._A.erase(srv._A.begin() + j);
							srv._closeConnection = -1;
						}
					}
				}
			}
			if (srv._pollFds[i].revents & POLLOUT)
			{
				for (size_t j = 0; j < srv._A.size(); j++)
				{
					if (srv._pollFds[i].fd == srv._A[j]->getFd())
					{
						srv._A[j]->action();
						if (srv.closeConnection() == srv._pollFds[i].fd)
						{
							delete srv._A[j];
							srv._pollFds.erase(srv._pollFds.begin() + i);
							srv._A.erase(srv._A.begin() + j);
							srv._closeConnection = -1;
						}
					}
				}
			}
		}
	}
}
