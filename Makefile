# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2021/04/26 15:17:55 by vkuikka          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = dnukem
SRCDIR = src/
SRC = main.c color.c culling.c culling_occlusion.c vectors.c init.c raycast.c obj_read.c bmp_read.c find_quads.c wireframe.c ui.c ui_config.c physics.c enemies.c shaders.c select.c serialize.c screen_space_partition.c uv_editor.c uv_overlap.c 2d_intersect.c
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
	gcc $(FLAGS) -O3 -fsanitize=address -framework SDL2 -framework SDL2_ttf -F ./ $(FILES) $(LIB) -I $(INCLUDE) -o $(NAME) -rpath @executable_path
endif

clean:
#@make clean -C libft

fclean: clean
	rm -f $(NAME)
#@make fclean -C libft

re: fclean
	make all
