# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/25 23:52:35 by lupayet           #+#    #+#              #
#    Updated: 2026/09/05 14:44:00 by lupayet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

DEBUG ?= 1

GG = c++ -Wall -Werror -Wextra -std=c++98 -g -DDEBUG=$(DEBUG)

OBJ_D = ./obj/
SRC_D = ./src/
CFG_D = ./Config/
INC = ./inc/
INCLUDES = ./_includes/

CXXFLAGS = -I$(INC) -I$(INCLUDES)

SRC = main.cpp AAction.cpp Client.cpp WebServ.cpp Socket.cpp Responce.cpp \
	WebPage.cpp Header.cpp Cgi.cpp Index.cpp Get.cpp Redirection.cpp Helper.cpp

HEADER = webserv.hpp AAction.hpp Client.hpp WebServ.hpp Socket.hpp WebPage.hpp

CFG_SRC = Config.cpp \
	ConfigBuilder.cpp \
	Location.cpp \
	LocationBuilder.cpp \
	Server.cpp \
	ServerBuilder.cpp \
	__internal__.cpp

INCLUDE_HEADER = Colors.hpp \
	ConfigBuilder.hpp \
	Config.hpp \
	LocationBuilder.hpp \
	Location.hpp \
	ServerBuilder.hpp \
	Server.hpp \
	__internal__.hpp

OBJ = $(addprefix $(OBJ_D), $(SRC:.cpp=.o)) \
	$(addprefix $(OBJ_D), $(CFG_SRC:.cpp=.o))

DEPS = $(addprefix $(INC), $(HEADER)) \
	$(addprefix $(INCLUDES), $(INCLUDE_HEADER))

all: $(NAME)

$(OBJ_D):
	@mkdir -p $(OBJ_D)

# src/*.cpp -> obj/*.o
$(addprefix $(OBJ_D), $(SRC:.cpp=.o)): $(OBJ_D)%.o: $(SRC_D)%.cpp $(DEPS)
	$(GG) $(CXXFLAGS) -c $< -o $@

# Config/*.cpp -> obj/*.o
$(addprefix $(OBJ_D), $(CFG_SRC:.cpp=.o)): $(OBJ_D)%.o: $(CFG_D)%.cpp $(DEPS)
	$(GG) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(GG) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

dev: re
	@make clean 1>/dev/null

.PHONY: all clean fclean re dev