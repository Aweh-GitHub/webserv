/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAction.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 20:43:31 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/09 12:45:48 by thantoni         ###   ########.fr       */
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
	protected:
		int	_fd;
};
