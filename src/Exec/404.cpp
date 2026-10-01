/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   404.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 02:20:46 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/30 08:48:19 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "core_webserv.hpp"
#include <sstream>

void	Client::ErrorResponce(int error)
{
	std::string	path;
	std::string	body;
	std::map<size_t, std::string>::const_iterator it;

	it = std::map<size_t, std::string>::const_iterator();
	if (_serverOrigin != NULL)
		it = _serverOrigin->GetErrorPages().find(static_cast<size_t>(error));
	if (_serverOrigin != NULL &&
		it != _serverOrigin->GetErrorPages().end())
		{
			path = it->second;
			if(!internalRedirection(path))
			{
				std::ostringstream defaultPage;
				defaultPage << "./template/" << error << ".html";
				path = defaultPage.str();
			}
			else
				return ;
		}

	if (!getFileContent(path, body))
	{
		body = "<html><body><h1><center>";
		body += ft_itoa(error);
		body += "</center></h1></body></html>";
	}

	_resHeader = getHeader(error, "text/html", body.length());
	_resBody += body;
	_status = SENDING;
}

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