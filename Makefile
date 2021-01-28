# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2021/01/26 02:26:08 by vkuikka          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = dnukem
SRCDIR = src/
SRC = main.c color.c culling.c vectors.c init.c render.c obj_read.c bmp_read.c find_quads.c wireframe.c
LIB = libft/libft.a
INCLUDE = libft/includes
#FLAGS = -Wall -Wextra -Werror

.PHONY: clean fclean re all
FILES=$(addprefix src/, $(SRC))

all: $(NAME)

$(NAME):
	#@make -C libft
	gcc $(FLAGS) -O3 -framework SDL2 -F ./ $(FILES) $(LIB) -I $(INCLUDE) -o $(NAME) -rpath @executable_path

clean:
	#@make clean -C libft

fclean: clean
	rm -f $(NAME)
	#@make fclean -C libft

re: fclean
	make all
