# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maki <maki@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/07/05 11:46:43 by ysan-seb          #+#    #+#              #
#    Updated: 2019/07/06 22:32:56 by maki             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = matt_daemon

SRC_PATH = ./srcs/

SRCS =	$(SRC_PATH)main.cpp\
		$(SRC_PATH)signals.cpp \
		$(SRC_PATH)Tintin_reporter.cpp \

OBJS =	$(SRCS:.cpp=.o)

CC = g++

FLAGS = -Wall -Wextra -Werror -std=c++11

HEADER = -I./inc

RM = rm -rf

all	: $(NAME)

$(NAME):$(OBJS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJS) $(HEADER)

%.o: %.cpp ./inc/Tintin_reporter.hpp ./inc/matt_daemon.hpp
	$(CC) $(FLAGS) -o $@ -c  $< $(HEADER)

clean	:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re	:	fclean
	make

.PHONY: all lib clean fclean re
