/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAction.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:43:31 by lupayet           #+#    #+#             */
/*   Updated: 2026/08/27 09:11:51 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class AAction
{
	public:
		virtual		~AAction();
		virtual void	action() = 0;
		virtual int	getFd();
	protected:
		int	_fd;
};
