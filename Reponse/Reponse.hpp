/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:06:09 by hanna             #+#    #+#             */
/*   Updated: 2026/06/13 19:35:00 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPONSE_HPP
#define REPONSE_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include "../Client/Client.hpp"

class Reponse 
{
    private:
        int status;
        std::string body;
        std::string headers;
    public:
        //int GetStatus ();
        Reponse() : status(0) {}
        void setStatus(int code);
        void setBody(std::string b);
        std::string build();
};
#endif