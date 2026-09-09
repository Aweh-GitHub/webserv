# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: thantoni <thantoni@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/09 12:18:45 by thantoni          #+#    #+#              #
#    Updated: 2026/09/09 12:47:42 by thantoni         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



DEBUG ?= 1



NAME					=	webserv

COMPILE					=	c++
FLAGS					=	-Wall -Wextra -Werror -std=c++98 -DDEBUG=$(DEBUG)

INCLUDE_PATH__ROOT		=	_includes
INCLUDE_PATH__CONFIG	=	_includes/Config
INCLUDE_PATH__EXEC		=	_includes/Exec
INCLUDES				=	-I $(INCLUDE_PATH__ROOT) -I $(INCLUDE_PATH__CONFIG) -I $(INCLUDE_PATH__EXEC)

RM						=	rm -rf

SRCS__CONFIG			=														\
							src/Config/Config.cpp								\
							src/Config/ConfigBuilder.cpp						\
							src/Config/Server.cpp								\
							src/Config/ServerBuilder.cpp						\
							src/Config/Location.cpp								\
							src/Config/LocationBuilder.cpp						\
							src/Config/__internal__.cpp							\

SRCS__EXEC				=														\
							src/Exec/AAction.cpp								\
							src/Exec/Cgi.cpp									\
							src/Exec/Client.cpp									\
							src/Exec/Get.cpp									\
							src/Exec/Header.cpp									\
							src/Exec/Helper.cpp									\
							src/Exec/Index.cpp									\
							src/Exec/Redirection.cpp							\
							src/Exec/Response.cpp								\
							src/Exec/Socket.cpp									\
							src/Exec/WebPage.cpp								\
							src/Exec/WebServ.cpp								\

SRCS			=																\
							src/main.cpp										\
							$(SRCS__CONFIG)										\
							$(SRCS__EXEC)										\

OBJS			= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(COMPILE) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(COMPILE) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re