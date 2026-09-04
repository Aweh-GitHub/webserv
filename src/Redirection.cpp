/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Redirection.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 07:55:05 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 08:18:45 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

std::string	Client::redirect(int code)
{
	
	std::string	redirection;

	redirection += "HTTP/1.1 ";
	redirection += redirMap(code);
	redirection += "\r\n";
	redirection += "Location: https://www.youtube.com/watch?v=dQw4w9WgXcQ\r\n";
	redirection += "Content-Length: 0\r\n\r\n";
	return (redirection);
}