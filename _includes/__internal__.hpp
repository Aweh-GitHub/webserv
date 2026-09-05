/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __internal__.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:07:48 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/04 15:07:08 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __INTERNAL___HPP
# define __INTERNAL___HPP

#include <sstream>
#include <string>
#include <vector>

bool			lineParseKeyValue(std::string& line, std::string& outKey, std::string& outValue);
bool			lineStripComment(std::string& line);
bool			isCommentLine(std::string& line);
std::string		lineStripQuotes(std::string& line);
bool			isLineEndBracket(std::string line);

size_t			toSize(const std::string& str);

std::vector<std::string> split(const std::string& str, char delimiter);

template <typename T> std::string toString(const T& value)
{
	std::ostringstream oss;

	oss << value;
	return (oss.str());
}

std::string	lineTrimSpaces(const std::string& str);

#endif
