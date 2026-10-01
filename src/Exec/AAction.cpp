/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAction.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 04:15:09 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 07:25:15 by lupayet          ###   ########.fr       */
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

int AAction::getCgiInputFd()
{
	return (-1);
}