/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 12:08:54 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/22 16:13:35 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <map>
# include <set>

class Location
{
	public:
		Location();
		~Location();

	public:
		void						Print() const;

		const std::string							GetName() const;
		const std::string							GetPathRoot() const;
		const std::vector<std::string>&				GetIndexFiles() const;
		const std::string							GetPathUploadStore() const;
		const std::set<std::string>&				GetAllowedMethods() const;
		const std::map<std::string, std::string>&	GetCGIExtensions() const;
		size_t										GetClientMaxBodySize() const;
		bool										GetAutoIndex() const;
		size_t										GetReturnCode() const;
		const std::string							GetReturnPath() const;

		void	SetName(const std::string name);
		void	SetPathRoot(const std::string pathRoot);
		void	SetIndexFiles(const std::vector<std::string> indexFiles);
		void	SetPathUploadStore(const std::string pathUploadStore);
		void	SetAllowedMethods(const std::set<std::string> allowedMethods);
		void	SetClientMaxBodySize(const size_t clientMaxBodySize);
		void	SetAutoIndex(const bool autoIndex);
		void	SetRedirectCode(const size_t redirectCode);
		void	SetRedirectPath(const std::string redirectPath);
		void	AddCGIExtension(const std::string ext, const std::string exec_path);
	
		bool	IsRedirection() const;
	private:
		std::string							_name;
		// PAGE LOCATION
		std::string							_pathRoot;
		std::vector<std::string>			_indexFiles;
		std::string							_pathUploadStore;
		std::set<std::string>				_allowedMethods;
		std::map<std::string, std::string>	_cgiExtensions; //std::map<"extension", "path_exec">
		size_t								_clientMaxBodySize;
		bool								_autoIndex;
		
		// RETURN LOCATION
		size_t								_redirectCode; // if -> returnCode == 0 -> n'est pas une location de redirection | else -> returnCode != 0 location de redirection
		std::string							_redirectPath;
};

#endif