/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   __internal__.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:07:48 by thantoni          #+#    #+#             */
/*   Updated: 2026/09/12 16:26:26 by thantoni         ###   ########.fr       */
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
bool			isSkipLine(std::string& line);
void			verifyQuotesSanity(std::string& line);
std::string		lineStripQuotes(std::string& line);
bool			isLineEndBracket(std::string line);

size_t			toSize(const std::string& str);

std::vector<std::string> splitValue(const std::string& str, char delimiter);

void	verifyIP(const std::string& ip);
size_t	parseAndVerifyPort(const std::string& value);
void	verifyDomain(const std::string& domain);
size_t	parseAndVerifyRedirectCode(const std::string& value);
void	verifyDirPath(const std::string& path);
void	verifyUploadDirPath(const std::string& path);
void	verifyURL(const std::string& path);
void	verifyRedirectURL(const std::string& path);
void	verifyIndexFileName(const std::string& fileName);
size_t	parseAndVerifyMaxBodySize(const std::string& value);
size_t	parseAndVerifyErrorCode(const std::string& value);
void	verifyHTTPMethod(const std::string& method);
void	verifyCGIExtension(const std::string& extension);
void	verifyCGIExecPath(const std::string& path);


template <typename T> std::string toString(const T& value)
{
	std::ostringstream oss;

	oss << value;
	return (oss.str());
}

std::string	lineTrimSpaces(const std::string& str);

void	throwLineError(const std::string& str);

#endif
