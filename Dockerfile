FROM debian:12-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        php-cgi \
        php-mysql \
        python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /webserv

COPY Makefile ./Makefile
COPY _includes ./_includes
COPY src ./src
COPY docker.config ./docker.config
COPY rendu.config ./rendu.config
COPY template ./template
COPY www ./www

RUN mkdir -p www/site0/root/uploads

RUN make re

RUN sed -i \
    -e "s#http://bob:8080/wordpress#http://localhost:8080/wordpress#g" \
    -e "s#CHANGE_THIS_TO_A_STRONG_PASSWORD#wordpress#g" \
    -e "s#DB_HOST', 'localhost#DB_HOST', 'database#g" \
    www/site1/wordpress/wp-config.php

EXPOSE 8080

CMD ["./webserv", "rendu.config"]