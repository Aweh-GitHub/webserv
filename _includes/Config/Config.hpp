/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:07:42 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/14 20:40:43 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include "Server.hpp"

class Config
{
	public:
		Config();
		~Config();
	public:
		const std::vector<Server>	GetAllServers();

		void						AddServer(const Server server);

		void						ToggleInitialized();
		bool						GetIsInitialized();

		void	Print() const;
		const Server* TryFindServer(const std::string& hostIp, const size_t listenPort, const std::string& serverDomain) const;
	private:
		std::vector<Server> _allServers;
		bool				_isInitialized;
};

#endif