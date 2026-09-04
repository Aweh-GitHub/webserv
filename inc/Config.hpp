/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/31 20:19:49 by lupayet           #+#    #+#             */
/*   Updated: 2026/08/31 21:22:18 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

enum	method
{
	GET,
	POST,
	DELETE,
	NOT
};

typedef struct	s_errorPage
{
	int			error;	//404
	std::string	path;	// "/www/errors/404.html"
}	t_errorPage;

typedef struct s_location
{
	int			redir;		// 301; 0 if no redir
	std::string	path;		// "/" "/about/"
	std::string	url;		// if redirection is a full url "http://www.example.com" else use path
	method		methods[3];	// {GET, POST, DELETE} > mean accept all; {GET, NOT, NOT} > only GET
	bool		cgi;		// if use a cgi true
	std::string	cgiPath; 	// "/usr/bin/php-cgi"
}	t_location;


class Config
{
private:
	int							_port;					// 8080
	std::vector<std::string>	_serverName;			// "localhost" "monsite.com"
	std::string					_rootPath;				// "/www/site1/"
	size_t						_client_max_body_size;	// 10 000 000 (10MB)
	std::vector<t_errorPage>	_errPage;
	std::vector<t_location>		_location;

public:
	Config();
	~Config();
};