/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAction.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 04:15:09 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/05 06:28:43 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AAction.hpp"

int	AAction::getFd()
{
	return (_fd);
}

int	AAction::getCgiFd()
{
	return (_cgiFd);
}

AAction::~AAction() {}

void AAction::action(int fd, short revents)
{
	(void)fd;
	(void)revents;
	action();
}

bool AAction::checkTimeout(bool pollTimedOut)
{
	(void)pollTimedOut;
	return (false);
}

int AAction::getCgiInputFd()
{
	return (-1);
}