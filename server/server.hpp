/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:14:46 by hanna             #+#    #+#             */
/*   Updated: 2026/07/18 16:01:58 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Requetes/Request.hpp"
#include "../Reponse/Reponse.hpp"
#include "../Client/Client.hpp"
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <vector>    
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

class Server
{
    private:
        std::vector<int> listen_fds;
        std::vector<pollfd> poll_fds;
    
    public:
        Server();
        void init();
        void run();
    
    private:
        int create_socket();
       int bind_Socket(int fd);
        int listen_SOcket(int fd);
        int accept_Client(int fd);
};

int Send_reponse(Client &client, Reponse &Reponse);
int pars_resaux(const char *buffer, Request &req);

#endif