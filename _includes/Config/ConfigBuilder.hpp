/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigBuilder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 19:14:16 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/10 14:56:12 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGBUILDER_HPP
# define CONFIGBUILDER_HPP

# include "Config.hpp"
# include <string>

class ConfigBuilder
{
	private:
		ConfigBuilder();
		~ConfigBuilder();
	public:
		static Config	ParseConfig(std::string& pathConfigFile);
		static int		LineIndex;
	private:
		static void	handleParse_Server(Config& config, std::string value, std::ifstream& file);
};

#endif