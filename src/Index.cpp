/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Index.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 08:05:16 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 10:11:35 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebPage.hpp"

std::string	Client::indexDir(std::string &path)
{
	WebPage	p(path);
	std::ostringstream body;

	body << "<ul>";
	DIR *dir = opendir(path.c_str());
	struct dirent	*ent = readdir(dir);
	while (ent != NULL)
	{
		body << "<li><a href=\"" << ent->d_name << "\">" << ent->d_name << "</li>";
		ent = readdir(dir);
	};
	body << "</ul>";
	p.addToBody(body.str());
	return (p.str());
}

bool Client::handleIndex(std::string &path)
{
	std::string	body = indexDir(path);
	_res += getHeader(200, "text/html", body.size());
	_res += body;
	return (true);
}