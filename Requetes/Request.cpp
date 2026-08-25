/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:05:32 by hanna             #+#    #+#             */
/*   Updated: 2026/06/13 15:04:01 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void Request::setMethod(const std::string& m)
{
    this->method = m;
}
void Request::setPath(const std::string& p)
{
    this->path = p;
}
void Request::setVersion(const std::string& v)
{
    this->version= v;
}
std::string Request::getMethod()
{
    return this->method;
}

std::string Request::getPath()
{
    return this->path;
}

std::string Request::getVersion()
{
    return this->version;
}