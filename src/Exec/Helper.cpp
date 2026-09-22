/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 07:49:25 by lupayet           #+#    #+#             */
/*   Updated: 2026/09/21 18:46:50 by lupayet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <fstream>
#include <sstream>
#include <map>

std::string load_file(const std::string &path) {
	std::ifstream file(path.c_str());
	std::ostringstream content;
	content << file.rdbuf(); // Read file into content
	return content.str(); // Return the string
}

int	getFileContent(std::string &filename, std::string &out)
{
	std::string	tmp;
	tmp = load_file(filename);
	if (!tmp.size())
		return 0;
	out += tmp;
	return 1;
}

std::string	ft_itoa(int n)
{
	std::ostringstream oss;
	oss << n;
	return (oss.str());
}

std::string getValue(const std::string& key, const std::map<std::string, std::string>& map)
{
    std::map<std::string, std::string>::const_iterator it = map.find(key);
    if (it != map.end())
        return it->second;

    return "";
}

std::string getMimeType(const std::string& path)
{
    size_t dot = path.find_last_of('.');

    if (dot == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dot + 1);

    if (ext == "html" || ext == "htm")
        return "text/html";

    if (ext == "css")
        return "text/css";

    if (ext == "js")
        return "application/javascript";

    if (ext == "txt")
        return "text/plain";

    if (ext == "jpg" || ext == "jpeg")
        return "image/jpeg";

    if (ext == "png")
        return "image/png";

    if (ext == "gif")
        return "image/gif";

    if (ext == "svg")
        return "image/svg+xml";

    if (ext == "ico")
        return "image/x-icon";

    if (ext == "pdf")
        return "application/pdf";

    if (ext == "json")
        return "application/json";

    if (ext == "xml")
        return "application/xml";

    if (ext == "mp3")
        return "audio/mpeg";

    if (ext == "mp4")
        return "video/mp4";

    if (ext == "webm")
        return "video/webm";

    if (ext == "wasm")
        return "application/wasm";

    return "application/octet-stream";
}

void splitUrl(const std::string &url, std::string &urlPath, std::string &urlQuery)
{
	std::string::size_type pos = url.find('?');

	if (pos == std::string::npos)
	{
		urlPath = url;
		urlQuery.clear();
		return;
	}

	urlPath = url.substr(0, pos);
	urlQuery = url.substr(pos + 1);
}

std::string removePort(const std::string& host)
{
    if (host.empty())
        return host;

    // IPv6: [::1]:8080
    if (host[0] == '[')
    {
        std::string::size_type end = host.find(']');
        if (end != std::string::npos)
            return host.substr(0, end + 1);
    }

    // IPv4 / hostname: example.com:8080
    std::string::size_type pos = host.find(':');
    if (pos != std::string::npos)
        return host.substr(0, pos);

    return host;
}