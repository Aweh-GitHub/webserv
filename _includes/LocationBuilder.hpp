/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 14:54:35 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/01 19:36:13 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONBUILDER_HPP
# define LOCATIONBUILDER_HPP

# include <fstream>
# include "Server.hpp"

class LocationBuilder
{
	private:
		LocationBuilder();
		~LocationBuilder();
	public:
		static Location	ParseLocation(std::ifstream& file);
	private:
		static std::map<std::string, void (*)(Location&, std::string)>	getParseHandlers();
		static void handleParse_Name(Location& location, std::string value);
		static void handleParse_PathRoot(Location& location, std::string value);
		static void handleParse_IndexFiles(Location& location, std::string value);
		static void handleParse_UploadStore(Location& location, std::string value);
		static void handleParse_AllowedMethods(Location& location, std::string value);
		static void handleParse_CGIExtension(Location& location, std::string value);
		static void handleParse_ClientMaxBodySize(Location& location, std::string value);
		static void handleParse_AutoIndex(Location& location, std::string value);
		static void handleParse_Redirect(Location& location, std::string value);
		// TODO: static void handleParse_CGIExtension();
};

#endif