/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reponse.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:07:13 by hanna             #+#    #+#             */
/*   Updated: 2026/06/13 19:37:13 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Reponse.hpp"

void Reponse::setStatus(int code)
{
    this->status = code;
}

void Reponse::setBody(std::string b)
{
    this->body = b;
}

std::string getStatusMessage(int status)
{
    if(status== 200)
        return( "OK");
    if(status == 404)
        return( "Not Found");   
    return "Internal Server Erro";    
}

std::string Reponse::build()
{
    std::stringstream ss;
    ss << "HTTP/1.1 " << this->status << " " << getStatusMessage(this->status) << "\r\n";
    ss << "Content-Length: " << this->body.size() << "\r\n";
    ss << "\r\n";
    ss << this->body;
    return ss.str();
}

int Send_reponse(Client &client, Reponse &Reponse)
{
    std::string reponse = Reponse.build();
    return (send(client.fd, reponse.c_str(), reponse.size(), 0));
}
// int Reponse::GetStatus()
// {
//     return(this->status);
// }

