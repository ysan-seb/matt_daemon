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
