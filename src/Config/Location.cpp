/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 12:10:26 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/22 16:13:40 by thantoni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "__internal__.hpp"
#include <iostream>

Location::Location() : _name(), _pathRoot(), _indexFiles(), _pathUploadStore(), _allowedMethods(), _clientMaxBodySize(1048576), _autoIndex(false), _redirectCode(0), _redirectPath()
{

}

Location::~Location() { }

void	Location::Print() const
{
	
	std::cout	<<	"\n\t[>] Location: \"" << this->_name << "\"\n"
				<<	"\t\tName\t\t\t: \""		<< this->_name	<< "\"\n"
				<<	"\t\tPath Root\t\t: \""	<< this->_pathRoot << "\"\n"
				<<	"\t\tIndex Files\t\t: ";
	for (size_t i = 0; i < this->_indexFiles.size(); ++i)
	{
		std::cout << "\"" << this->_indexFiles[i] << "\"";
		if (i + 1 < this->_indexFiles.size())
			std::cout << ", ";
	}
	std::cout	<<	"\n"
				<<	"\t\tPath Upload Store\t: \"" << this->_pathUploadStore << "\"\n"
				<<	"\t\tAllowed Methods\t\t: ";
	for (std::set<std::string>::const_iterator method = this->_allowedMethods.begin(); method != this->_allowedMethods.end(); ++method)
	{
		std::cout << "\"" << *method << "\" ";
	}
	std::cout	<< "\n"
				<<	"\t\tCGI Extensions\t\t: ";
	for (std::map<std::string, std::string>::const_iterator cgiIt = this->_cgiExtensions.begin(); cgiIt != this->_cgiExtensions.end(); ++cgiIt)
	{
		std::cout << "{ " << cgiIt->first << " : \"" << cgiIt->second << "\" }  ";
	}
	std::cout <<	"\n"
				<<	"\t\tClient Max Body Size\t: " << this->_clientMaxBodySize << "\n"
				<<	"\t\tAuto Index\t\t: " << (this->_autoIndex ? "(true)" : "(false)") << "\n";
}

const std::string							Location::GetName() const { return (this->_name); }
const std::string							Location::GetPathRoot() const { return (this->_pathRoot); }
const std::vector<std::string>&				Location::GetIndexFiles() const { return (this->_indexFiles); }
const std::string							Location::GetPathUploadStore() const { return (this->_pathUploadStore); }
const std::set<std::string>&				Location::GetAllowedMethods() const { return (this->_allowedMethods); }
const std::map<std::string, std::string>&	Location::GetCGIExtensions() const { return (this->_cgiExtensions); }
size_t										Location::GetClientMaxBodySize() const { return (this->_clientMaxBodySize); }
bool										Location::GetAutoIndex() const { return (this->_autoIndex); }
size_t										Location::GetReturnCode() const { return (this->_redirectCode); }
const std::string							Location::GetReturnPath() const { return (this->_redirectPath); }

void	Location::SetName(const std::string name) { this->_name = name; }
void	Location::SetPathRoot(const std::string pathRoot) { this->_pathRoot = pathRoot; }
void	Location::SetIndexFiles(const std::vector<std::string> indexFiles) { this->_indexFiles = indexFiles; }
void	Location::SetPathUploadStore(const std::string pathUploadStore) { this->_pathUploadStore = pathUploadStore; }
void	Location::SetAllowedMethods(const std::set<std::string> allowedMethods) { this->_allowedMethods = allowedMethods; }
void	Location::SetClientMaxBodySize(const size_t clientMaxBodySize) { this->_clientMaxBodySize = clientMaxBodySize; }
void	Location::SetAutoIndex(const bool autoIndex) { this->_autoIndex = autoIndex; }
void	Location::SetRedirectCode(const size_t redirectCode) { this->_redirectCode = redirectCode; }
void	Location::SetRedirectPath(const std::string redirectPath) { this->_redirectPath = redirectPath; }
void	Location::AddCGIExtension(const std::string ext, const std::string exec_path)
{
	std::pair<std::map<std::string, std::string>::iterator, bool> insertResult;

	insertResult = this->_cgiExtensions.insert(std::make_pair(ext, exec_path));
	if (!insertResult.second)
		throwLineError("Duplicate CgiExtension: " + ext);
}

bool	Location::IsRedirection() const { return (this->_redirectCode != 0); }