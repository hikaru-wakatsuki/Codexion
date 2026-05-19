# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/24 22:55:27 by hwakatsu          #+#    #+#              #
#    Updated: 2026/05/19 14:38:46 by hwakatsu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

INCDIR = includes

DIR = src

SRCS = 	coder_routine.c \
		dongles_utils.c \
		dongles.c \
		heap.c \
		main.c \
		monitor_routine.c \
		parse_args.c \
		sim_control.c \
		threads_control.c \
		utils.c \

SRCS := $(addprefix $(DIR)/, $(SRCS))
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clena fclean re
