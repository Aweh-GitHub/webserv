*This project has been created as part of the 42 curriculum by lupayet, thantoni.*

# Webserv

## Description

Webserv is a HTTP/1.1 server written in C++98 as part of the 42 curriculum. Its goal is to provide a working event-driven web server capable of accepting client connections, parsing HTTP requests, routing them according to a configuration file, and returning HTTP responses.

The server uses non-blocking sockets and `poll()` to handle multiple connections. It supports configurable virtual servers and locations, static files, directory indexes, custom error pages, redirects, request bodies, file uploads, and CGI execution.

## Features

- HTTP request parsing and response generation
- `GET`, `POST`, and `DELETE` methods
- Multiple servers and locations in one configuration file
- Host and port-based server selection
- Static file serving
- Configurable index files and automatic directory listing
- Custom error pages
- HTTP redirects
- Request body size limits
- File uploads
- CGI support, including PHP through `php-cgi`
- Event-driven connection management with `poll()`

## Instructions

### Requirements

- A Unix-like operating system
- A C++ compiler with C++98 support
- `make`
- Optional CGI interpreters matching the configured CGI extensions, such as `php-cgi`

### Compilation

From the repository root, compile the project with:

```sh
make
```

The executable is named `webserv`. The Makefile enables warnings as errors, C++98 mode, and AddressSanitizer by default.

Useful Makefile targets are:

```sh
make clean   # Remove object files
make fclean  # Remove object files and the executable
make re      # Rebuild the project
```

### Execution

The program requires exactly one configuration file:

```sh
./webserv dev.config
```

The included `dev.config` demonstrates two server definitions, locations for static content and WordPress, redirects, custom error pages, uploads, and PHP CGI. Update its IP addresses, ports, document roots, and interpreter paths to match the local environment before running it.

For example, the default configuration includes routes similar to:

```text
/          Static files with GET, POST, and DELETE support
/html      Static files with automatic directory listing
/wordpress PHP CGI files
/redir     HTTP redirection
```

The server can be stopped with `Ctrl+C`.

## Configuration

Configuration files use the project's custom syntax. A server can define a listening address and port, accepted domains, a document root, index files, an upload limit, error pages, and nested locations.

Example:

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
        autoIndex: "false"
    }
}
```

## Project Structure

- `_includes/`: C++ headers
- `src/Config/`: configuration parsing and server/location models
- `src/Exec/`: sockets, clients, request handling, responses, redirects, and CGI execution
- `errors/`: shared error-page templates
- `template/`: project templates
- `www/`: document roots, test websites, uploads, and CGI content
- `dev.config`: example development configuration
- `Makefile`: compilation and cleanup commands

## Resources

- [RFC 9110 - HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110)
- [RFC 9112 - HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112)
- [Linux `poll(2)` documentation](https://man7.org/linux/man-pages/man2/poll.2.html)
- [Linux socket programming overview](https://man7.org/linux/man-pages/man7/socket.7.html)
- [Common Gateway Interface specification](https://www.rfc-editor.org/rfc/rfc3875)
- [C++ reference](https://en.cppreference.com/w/cpp)

AI tools were used as a development aid for selected tasks, including clarifying HTTP and socket concepts, reviewing implementation ideas, suggesting test scenarios, and helping draft project documentation. The project code, configuration choices, debugging, and final verification were reviewed and integrated by the project authors.
