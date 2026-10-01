/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAction.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:43:31 by lupayet           #+#    #+#             */
/*   Updated: 2026/10/01 02:37:35 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core_webserv.hpp"

class AAction
{
	public:
		virtual		~AAction();
		virtual void	action() = 0;
		virtual int	getFd();
		virtual int	getCgiFd();
	protected:
		int	_fd;
		int	_cgiFd;
};
