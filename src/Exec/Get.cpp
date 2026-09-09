/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 08:02:25 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/04 10:11:42 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

bool Client::handleGet(std::string &path)
{
	std::string	err("./template/404.htlm");
	std::string	body;
	if (!getFileContent(path, body))
	{
		_res += getHeader(404, "text/html", body.size());
		getFileContent(err, body);
		_res += body;
		return (false);
	}
	_res += getHeader(200, getMimeType(path), body.size());
	_res += body;
	return (true);
}