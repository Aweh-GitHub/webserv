/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Index.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 08:05:16 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/30 08:36:49 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "WebPage.hpp"
#include <dirent.h>
#include <sstream>
#include <string>

static std::string	escapeHtml(const std::string &str)
{
	std::string out;

	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		switch (*it)
		{
			case '&':  out += "&amp;";  break;
			case '<':  out += "&lt;";   break;
			case '>':  out += "&gt;";   break;
			case '"':  out += "&quot;"; break;
			case '\'': out += "&#39;";  break;
			default:   out += *it;      break;
		}
	}
	return (out);
}

std::string	Client::indexDir(std::string &path)
{
	WebPage				p(path);
	std::ostringstream	body;
	DIR					*dir;
	struct dirent		*ent;

	dir = opendir(path.c_str());
	if (dir == NULL)
		return ("");

	body << "<ul>";

	while ((ent = readdir(dir)) != NULL)
	{
		std::string name = ent->d_name;

		// Don't expose the special directory entries.
		if (name == "." || name == "..")
			continue;

		std::string safeName = escapeHtml(name);

		body << "<li><a href=\""
			 << safeName
			 << "\">"
			 << safeName
			 << "</a></li>";
	}

	body << "</ul>";

	closedir(dir);

	p.addToBody(body.str());
	return (p.str());
}

bool	Client::handleIndex(std::string &path)
{
	std::string body = indexDir(path);

	if (body.empty())
		return (false);

	_resHeader += getHeader(200, "text/html", body.size());
	_resBody += body;
	return (true);
}