/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   404.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 02:20:46 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/18 22:47:13 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "core_webserv.hpp"
#include <sstream>

void	Client::notFoundRes(int errCode, std::map<size_t, std::string> errorPages)
{
	std::string		path;
	std::string		body;

	std::map<size_t, std::string>::iterator it = errorPages.find(errCode);

	if (it != errorPages.end())
		path = "." + it->second;
	else
	{
		std::ostringstream defaultPage;

		defaultPage << "./template/" << errCode << ".html";
		path = defaultPage.str();
	}

	std::cout << path << std::endl;
	if (!getFileContent(path, body))
	{
		body = "<html><body><h1>" 
			+ ft_itoa(errCode) 
			+ "</h1></body></html>";
	}

	_res = getHeader(errCode, "text/html", body.length());
	_res += body;
}