/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 00:51:43 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/09 12:47:10 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core_webserv.hpp"
#include "AAction.hpp"

class	Socket : public AAction
{
	public:
		Socket(int fd, int port);
		Socket(const Socket &cpy);
		Socket	&operator=(const Socket &other);
		~Socket();
		void	action();
		int		getPort();
	private:
		int		_port;
		Socket();
};
