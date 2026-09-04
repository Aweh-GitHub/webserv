# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lupayet <lupayet@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/25 23:52:35 by lupayet           #+#    #+#              #
#    Updated: 2026/09/03 14:25:02 by lupayet          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

DEBUG ?= 1

GG = c++ -Wall -Werror -Wextra -std=c++98 -g -DDEBUG=$(DEBUG)

OBJ_D = ./obj/
SRC_D = ./src/
INC = ./inc/

SRC = main.cpp AAction.cpp Client.cpp Server.cpp Socket.cpp Responce.cpp WebPage.cpp Header.cpp
HEADER = webserv.hpp AAction.hpp Client.hpp Server.hpp Socket.hpp Responce.hpp WebPage.hpp

OBJ = $(addprefix $(OBJ_D), $(SRC:.cpp=.o))
DEPS = $(addprefix $(INC), $(HEADER))

all : $(NAME)

$(OBJ_D):
	@mkdir -p $(OBJ_D)
$(OBJ_D)%.o: $(SRC_D)%.cpp $(DEPS)
	$(GG) -I$(INC) -c $< -o $@

$(NAME): $(OBJ_D) $(OBJ)
	$(GG) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:fclean all

dev: re
	@make clean 1>/dev/null

.PHONY: all clean fclean re dev
