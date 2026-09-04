/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebPage.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 06:36:40 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/03 06:48:20 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebPage.hpp"
#include <sstream>

WebPage::WebPage(const std::string &title) : _title(title) {}

WebPage &WebPage::addToHead(const std::string &html)
{
	_head += html;
	_head += "\n";

	return *this;
}

WebPage &WebPage::addToBody(const std::string &html)
{
	_body += html;
	_body += "\n";

	return *this;
}

std::string WebPage::str() const
{
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
		 << "<html>\n"
		 << "<head>\n"
		 << "    <meta charset=\"UTF-8\">\n"
		 << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		 << "    <title>" << _title << "</title>\n"
		 << _head
		 << "</head>\n"
		 << "<body>\n"
		 << _body
		 << "</body>\n"
		 << "</html>\n";

	return html.str();
}