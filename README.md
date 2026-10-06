*This project has been created as part of the 42 curriculum by lupayet, thantoni, hmimouni*

# Webserv

## Description
Webserv is a HTTP/1.1 server written in C++98 as part of the 42 curriculum.

### Goal
Its goal is to provide a working event-driven web server. Parsing tailored config file, accepting client connections, parsing HTTP requests, routing them according and returning HTTP responses.

### Specificities
The server uses non-blocking sockets and `poll()` to handle multiple connections. It supports configurable virtual servers and locations, static files, directory indexes, custom error pages, redirects, request bodies, file uploads, and CGI execution.

### Features
- HTTP request parsing and response generation
- `GET`, `POST`, and `DELETE` methods
- Multiple servers and locations in one configuration file
- Host and port-based server selection
- Static file serving
- Configurable index files and auto-index (automatic directory listing)
- Custom error pages
- HTTP redirects
- Request body size limits
- File uploads
- CGI support, including PHP through `php-cgi`
- Event-driven connection management with `poll()`

## Instructions

### Requirements
- Unix based operating system
- C++ compiler with C++98 support
- Makefile support
- Optional: CGI interpreters matching your config (ex: Php, Pyhton etc...)

### Quick Start
1. At root `make`
2. Create your own config file following the "Webserv Config" section below
3. Start the web server `./webserv <your_config_file>.config`
4. You can stop server at anytime with `Ctrl+C`

## Resources

### Makefile Commands
```sh
make clean   # Delete compilation files (.o)
make fclean  # Delete all compilation files (.o + executable)
make re      # Rebuild: Clean + Build 
```

### Webserv Config
Config files handle multiple servers declarations

#### Servers
Server files handle multiple locations declarations

- `server` Expects start delimiter on line `{` and end delimiter `}` on dedicated line
```
server:{

#! SERVER CONTENT

}
```
- `hostIp` Expects IP format quoted `"`
```
hostIp:"127.0.0.1"
```
- `listenPort` Expects port format non-quoted
```
listenPort:8080
```
- `serverDomains` Expects domains or IP quoted `"`, sperated by `,`
```
serverDomains:"localhost", "www.monsite.local", "127.0.0.1"
```
- `pathRoot` Expects root location
```
pathRoot:"./www/site"
```
- `indexFiles` Expects index files (left to right fallback) quoted `"`, separated by `,`
```
indexFiles:"index.html","index.htm"
```
- `clientMaxBodySize` Expects a size for max HTTP body non-quoted
```
clientMaxBodySize:10485760
```
- `errorPage` Expects error non-quoted separated by `,` and ending with error page path quoted `"`
```
#! single:  <error>, <errorPage>
errorPage:404,"/errors/404.html"

#! multi in-line:  <error>, <errorPage>
errorPage:500,502,503,"/errors/50x.html"
```
#### Locations
- `location` Expects start delimiter on line `{` and end delimiter `}` on dedicated line
```
location:{

#! LOCATION CONTENT

}
```
- `name` Expects name quoted `"`
```
name:"/"
```
- `pathRoot` Expects path to root quoted `"`
```
pathRoot:"./www/siteExample/root"
```
- `indexFiles` Expects index files (left to right fallback) quoted `"`, separated by `,`
```
indexFiles:"index.html","index.htm"
```
- `allowedMethods` Expects `GET` and/or `POST` and/or `DELETE` mehtods quoted `"`, separated by `"`
```
allowedMethods:"GET","POST","DELETE"
```
- `autoIndex` Expects `true` or `false` quoted `"`
```
autoIndex:"false"
```
- `pathUploadStore` Expects path to store uploads quoted `"`
```
pathUploadStore:"./www/uploads"
```

- `cgiExtension` Expects file-extension and execution path
```
cgiExtension:".sh","/usr/bin/bash"
```
- `redirect` Expects redirection-code and URL
```
redirect:301, "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
```

#### Example file:
```text
server: {
    hostIp: "127.0.0.1"
    listenPort: 8080
    serverDomains: "localhost"
    pathRoot: "./www/site0/root"
    indexFiles: "index.html"
    clientMaxBodySize: 10485760
    location: {
        name: "/"
        pathRoot: "./www/site0/root"
        allowedMethods: "GET", "POST", "DELETE"
        autoIndex: "true"
    }
    location:{
        name:"/redirlocal"
        redirect:301, "/"
    }
	location:{
		name:"/redir"
		redirect:301, "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
	}
	location:{
		name:"/timeout"
		pathRoot:"./www/site0/timeout"
		indexFiles:"timeout.php"
		allowedMethods:"GET"
		autoIndex:"false"
		cgiExtension:".php","/usr/bin/php-cgi"
	}
}
```

### Links
- [Nginx Config](https://nginx.org/en/docs/)
- [RFC 9110 - HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110)
- [RFC 9112 - HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112)
- [Linux *poll(2)* documentation](https://man7.org/linux/man-pages/man2/poll.2.html)
- [Linux socket programming overview](https://man7.org/linux/man-pages/man7/socket.7.html)
- [Common Gateway Interface specification](https://www.rfc-editor.org/rfc/rfc3875)
- [C++ reference](https://en.cppreference.com/w/cpp)

### AI Usage
AI tools were used as a development aid for selected tasks, including clarifying HTTP and socket concepts, reviewing implementation ideas, suggesting test scenarios, and helping draft project documentation.
