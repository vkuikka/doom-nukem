# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2021/04/06 15:48:12 by rpehkone         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = dnukem
SRCDIR = src/
SRC = main.c color.c culling.c culling_occlusion.c vectors.c init.c raycast.c obj_read.c bmp_read.c find_quads.c wireframe.c ui.c ui_config.c physics.c enemies.c shaders.c select.c serialize.c
LIB = libft/libft.a -L x86_64-w64-mingw32/lib/
INCLUDE = libft/includes -I x86_64-w64-mingw32/include/
#FLAGS = -Wall -Wextra -Werror

.PHONY: clean fclean re all
FILES=$(addprefix src/, $(SRC))

all: $(NAME)

$(NAME):
#@make -C libft
ifeq ($(OS),Windows_NT)
	gcc $(FLAGS) -O3  $(FILES) $(LIB) -I $(INCLUDE) -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lws2_32 -o $(NAME)
else
	gcc $(FLAGS) -O3 -framework SDL2 -framework SDL2_ttf -F ./ $(FILES) $(LIB) -I $(INCLUDE) -o $(NAME) -rpath @executable_path
endif

clean:
#@make clean -C libft

fclean: clean
	rm -f $(NAME)
#@make fclean -C libft

re: fclean
	make all
