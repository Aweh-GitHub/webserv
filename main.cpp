/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 18:59:31 by hanna             #+#    #+#             */
/*   Updated: 2026/07/18 15:57:52 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "server/server.hpp"


int pars_resaux(const char *buffer, Request &requetes)
{
    std::stringstream ss;
    std::string method;
    std::string path;
     std::string version;

    ss << buffer;
    ss >> method; 
    ss >> path;
    ss >> version;
    
    requetes.setMethod(method);
    requetes.setPath(path);
    requetes.setVersion(version);
    return 0;
}


int main()
{
    Server server;

    server.init();
    server.run();
    return 0;
}