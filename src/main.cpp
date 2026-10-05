/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 00:36:24 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/05 06:28:55 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core_webserv.hpp"
#include "WebServ.hpp"
#include "Colors.hpp"
#include "ConfigBuilder.hpp"
#include <limits>

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
		WebServ::_config = ConfigBuilder::ParseConfig(pathConfigFile);
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << RST << '\n';
	}
	signal(SIGINT, handleSignal);
	if (!srv.init())
		return (1);
	// std::cout << "Current poll fds: ";
	// 		for (size_t k = 0; k < srv._pollFds.size(); ++k)
	// 		{
	// 			std::cout << srv._pollFds[k].fd;
	// 			if (k < srv._pollFds.size() - 1)
	// 				std::cout << ", ";
	// 		}
	// 		std::cout << std::endl;
	while (srv._isRunning)
	{
		int pollResult = poll(srv._pollFds.data(), srv._pollFds.size(), 1000);
		bool pollTimedOut = (pollResult == 0);
		for (size_t j = 0; j < srv._A.size(); ++j)
		{
			int cgiFd = srv._A[j]->getCgiFd();
			int cgiInputFd = srv._A[j]->getCgiInputFd();

			if (!srv._A[j]->checkTimeout(pollTimedOut))
				continue;
			for (size_t k = 0; k < srv._pollFds.size(); )
			{
				if (srv._pollFds[k].fd == cgiFd ||
					srv._pollFds[k].fd == cgiInputFd)
					srv._pollFds.erase(srv._pollFds.begin() + k);
				else
					++k;
			}
		}
		for (size_t i = 0; i < srv._pollFds.size(); )
		{
			const int fd = srv._pollFds[i].fd;
			const short revents = srv._pollFds[i].revents;
			bool removed = false;

			if (revents & (POLLIN | POLLOUT | POLLHUP))
			{
				#ifndef DEBUG
				if (revents & POLLIN)
					std::cout << "POLLIN event on fd: " << fd << std::endl;
				if (revents & POLLOUT)
					std::cout << "POLLOUT event on fd: " << fd << std::endl;
				if (revents & POLLHUP)
					std::cerr << RED << "POLLHUP event on fd: " << fd << RST << std::endl;
				#endif
				for (size_t j = 0; j < srv._A.size(); j++)
				{
					bool isClientFd = (fd == srv._A[j]->getFd());
					bool isCgiFd = (fd == srv._A[j]->getCgiFd());
					bool isCgiInputFd = (fd == srv._A[j]->getCgiInputFd());

					if (isClientFd || isCgiFd || isCgiInputFd)
					{
						srv._A[j]->action(fd, revents);
						if (isCgiFd && srv._A[j]->getCgiFd() == -1)
						{
							srv._pollFds.erase(srv._pollFds.begin() + i);
							removed = true;
						}
						else if (isCgiInputFd && srv._A[j]->getCgiInputFd() == -1)
						{
							srv._pollFds.erase(srv._pollFds.begin() + i);
							removed = true;
						}
						else if (isClientFd && srv.closeConnection() == fd)
						{
							srv._pollFds.erase(srv._pollFds.begin() + i);
							delete srv._A[j];
							srv._A.erase(srv._A.begin() + j);
							srv._closeConnection = -1;
							close(fd);
							removed = true;
						}
						break;
					}
				}
			}
			else if (revents & (POLLERR | POLLNVAL))
			{
				#ifndef DEBUG
				if (revents & POLLERR)
					std::cerr << RED << "POLLERR event on fd: " << fd << RST << std::endl;
				if (revents & POLLNVAL)
					std::cerr << RED << "POLLNVAL event on fd: " << fd << RST << std::endl;
				#endif
				for (size_t j = 0; j < srv._A.size(); j++)
				{
					if (fd == srv._A[j]->getFd())
					{
						delete srv._A[j];
						srv._pollFds.erase(srv._pollFds.begin() + i);
						srv._A.erase(srv._A.begin() + j);
						removed = true;
						break;
					}
				}
			}
			if (!removed)
				++i;
			// std::cout << "Current poll fds: ";
			// for (size_t k = 0; k < srv._pollFds.size(); ++k)
			// {
			// 	std::cout << srv._pollFds[k].fd;
			// 	if (k < srv._pollFds.size() - 1)
			// 		std::cout << ", ";
			// }
			// std::cout << std::endl;
		}
	}
}
