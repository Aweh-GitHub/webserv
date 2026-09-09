/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:07:42 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/01 20:29:52 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include "Server.hpp"

class Config
{
	public:
		Config(const std::string& pathConfigFile);
		~Config();
	public:
		const std::vector<Server>	GetAllServers();

		void						AddServer(const Server server);

		void	Print() const;
	private:
		const std::string&	_pathConfigFile;
		std::vector<Server> _allServers;
};

#endif