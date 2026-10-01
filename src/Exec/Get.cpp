/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 08:02:25 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 05:54:16 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

bool Client::handleGet(std::string &path)
{
	std::string	err("./template/404.html");
	if (!getFileContent(path, _resBody))
	{
		ErrorResponce(404);
		return (false);
	}
	_resHeader = getHeader(200, getMimeType(path), _resBody.size());
	//_res += _resBody;
	return (true);
}