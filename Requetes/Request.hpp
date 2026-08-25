/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanna <hanna@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 21:06:45 by hanna             #+#    #+#             */
/*   Updated: 2026/06/13 19:14:19 by hanna            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <iostream>

class Request
{
    private:
        std::string method;
        std::string path;
        std::string version;
    public:
        void setMethod(const std::string& m);
        void setPath(const std::string& p);
        void setVersion(const std::string& v);
        

        std::string getMethod();
        std::string getPath();
        std::string getVersion();
};

#endif