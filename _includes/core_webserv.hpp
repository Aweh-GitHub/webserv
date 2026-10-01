/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_webserv.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 00:39:55 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 04:12:25 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef DEBUG
# define DEBUG 0
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#include <signal.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/select.h>

#include <sys/stat.h>
#include <dirent.h>
#include <poll.h>
#include <vector>
#include <map>

std::string	ft_itoa(int n);
int			getFileContent(std::string &filename, std::string &out);
std::string getValue(const std::string& key, const std::map<std::string, std::string>& map);
std::string getMimeType(const std::string& path);
void		splitUrl(const std::string &url, std::string &urlPath, std::string &urlQuery);
std::string removePort(const std::string& host);
bool		isSafeRequestPath(const std::string &path);
void		updatePollEvent(int fd, short events);
