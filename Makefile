# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/04/24 22:55:27 by hwakatsu          #+#    #+#              #
#    Updated: 2026/05/07 02:06:09 by hwakatsu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

INCDIR = includes

DIR = src

SRCS = 	coders.c \
		dongles_release.c \
		dongles_take.c \
		main.c \
		monitor_control.c \
		parse_args.c \
		request.c \
		sim_control.c \
		time.c \

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
