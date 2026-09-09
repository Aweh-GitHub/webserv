/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 00:39:55 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/09 12:39:41 by thantoni         ###   ########.fr       */
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