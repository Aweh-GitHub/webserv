/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 08:02:25 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/30 01:52:44 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


bool Client::handleGet(std::string &path)
{
	std::string	err("./template/404.html");
	//std::string	body;
	if (!getFileContent(path, _resBody))
	{
		_resHeader += getHeader(404, "text/html", _resBody.size());
		getFileContent(err, _resBody);
		//_res += _resBody;
		return (false);
	}
	_resHeader = getHeader(200, getMimeType(path), _resBody.size());
	//_res += _resBody;
	return (true);
}