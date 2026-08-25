/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:10:36 by hanna             #+#    #+#             */
/*   Updated: 2026/07/18 15:59:27 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "../Requetes/Request.hpp"
#include "../Client/Client.hpp"

int Server::create_socket()
{
    int fd = socket(AF_INET, SOCK_STREAM,0);
    if( fd < 0)
    {
        std::cout << "Error socket\n";
        return -1;
    }
    std::cout << "socket created :" << fd << std::endl;
    return fd;
}

int Server::bind_Socket(int fd)
{
    int opt = 1;
    
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        std::cout << "Error setsockopt\n";
        return -1;
    }
    sockaddr_in adresse;

    adresse.sin_port = htons(8080);
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
     if (bind(fd, (sockaddr*) &adresse,sizeof(adresse)) < 0)
    {
        std::cout << "Error bind : " << strerror(errno) << std::endl;        return -1;
    }
    std::cout << "bind ok \n";
    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}

int Server::listen_SOcket(int fd)
{
    if (listen(fd, 1000) < 0)
    {
        std::cout << "Error listen\n";
        return -1;
    }
    std::cout << "listen ok \n";
    return fd;
}

int Server::accept_Client(int fd)
{
    int fd_client;
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    fd_client = accept(fd, (sockaddr*) &client_addr, &len);
    if(fd_client < 0)
    {
        std::cout << "Error accept\n";
        return -1;
    }
    std::cout << "accept ok \n";
    return fd_client;
}

Server::Server()
{
}

void Server::init()
{
    int fd = create_socket();
    fd = bind_Socket(fd);
    fd = listen_SOcket(fd);
    fcntl(fd, F_SETFL, O_NONBLOCK); // porte non bloquante;
    this->listen_fds.push_back(fd);
    pollfd pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    this->poll_fds.push_back(pfd);
    
}
void Server::run()
{
    while(true)
    {
        poll(poll_fds.data(), poll_fds.size(), -1);

        for (size_t i =0; i< poll_fds.size(); i++)
        {
            if(poll_fds[i].revents &POLLIN)
            {
                bool is_listen_fd = false;
                for(size_t j =0; j< listen_fds.size(); j++)
                {
                    if(poll_fds[i].fd == listen_fds[j])
                    {
                        is_listen_fd = true;
                        break;
                    }
                }
                if(is_listen_fd)
                {
                    int new_client_fd = accept_Client(poll_fds[i].fd);
                    if(new_client_fd >=0)
                    {
                        fcntl(new_client_fd, F_SETFL, O_NONBLOCK);
                        pollfd client_pfd;
                        client_pfd.fd = new_client_fd;
                        client_pfd.events = POLLIN;
                        client_pfd.revents = 0;
                        poll_fds.push_back(client_pfd);   
                        std::cout << "Nouveau client accepté : " << new_client_fd << std::endl;
                        
                    }
                }
                else 
                {
                    char buffer[8192];
                    int n = recv(poll_fds[i].fd, buffer, sizeof(buffer) -1, 0);
                    if(n <= 0)
                    {
                        std::cout << "Client " << poll_fds[i].fd << "deconnecte"<< std::endl;
                        close(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin()+i);
                        i--;
                    }
                    else 
                    {
                        buffer[n] = '\0';
                        Request requete;
                        pars_resaux(buffer, requete);
                        std::cout << "Methode : " << requete.getMethod() << std::endl;
                        std::cout << "Path    : " << requete.getPath() << std::endl;
                        std::cout << "Version : " << requete.getVersion() << std::endl;
                    }
                }
            }
        }
    }
}
