/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAction.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:43:31 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 07:25:14 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core_webserv.hpp"

class AAction
{
	public:
		virtual		~AAction();
		virtual void	action() = 0;
		virtual void	action(int fd, short revents);
		virtual int	getFd();
		virtual int	getCgiFd();
		virtual int	getCgiInputFd();
	protected:
		int	_fd;
		int	_cgiFd;
};
