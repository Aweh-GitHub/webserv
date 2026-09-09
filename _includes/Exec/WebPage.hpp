/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebPage.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 06:24:53 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/03 06:48:23 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sstream>

class WebPage
{
	public:
		WebPage(const std::string& title);
		WebPage& addToHead(const std::string& html);
		WebPage& addToBody(const std::string& html);
		std::string str() const;
	private:
		std::string _title;
		std::string _head;
		std::string _body;
};