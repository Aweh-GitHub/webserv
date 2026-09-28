/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 00:51:43 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/28 23:19:37 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core_webserv.hpp"
#include "AAction.hpp"

class	Socket : public AAction
{
	public:
		Socket(int fd, size_t port, const std::string &ip);
		~Socket();
		void	action();
		size_t	getPort();
		std::string	&getIp();
	private:
		size_t		_port;
		std::string	_ip;
		Socket();
		Socket(const Socket &cpy);
		Socket	&operator=(const Socket &other);
};
