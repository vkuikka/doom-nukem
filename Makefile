# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2021/08/13 12:56:25 by rpehkone         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = dnukem
SRCDIR = src/
SRC = main.c input.c game_logic.c color.c culling.c culling_occlusion.c vectors.c init.c raycast.c obj_read.c bmp_read.c find_quads.c wireframe.c editor_set_state.c physics.c enemies.c shaders.c select.c serialize.c screen_space_partition.c uv_overlap.c 2d_intersect.c obj_editor.c door_runtime.c bake.c spray.c audio.c cast_face.c
SRC_UI = door_editor.c gizmo.c hud.c light_edit.c main_menu.c nonfatal_error.c ui_config.c ui_elements.c ui.c uv_editor.c
LIB = libft/libft.a -L x86_64-w64-mingw32/lib/
INCLUDE = libft/includes -I src/ -I x86_64-w64-mingw32/include/ -I x86_64-w64-mingw32/include/SDL2
SDL_FRAMEWORKS = -framework SDL2 -framework SDL2_ttf -framework SDL2_mixer
SDL_HEADER = -I SDL2.framework/Headers
#FLAGS = -Wall -Wextra -Werror

.PHONY: clean fclean re all
UI_FILES=$(addprefix ui/, $(SRC_UI))
SRC += $(UI_FILES)
FILES=$(addprefix src/, $(SRC))

all: $(NAME)

$(NAME):
#@make -C libft
ifeq ($(OS),Windows_NT)
	gcc $(FLAGS) -O3  $(FILES) $(LIB) -I $(INCLUDE) -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lws2_32 -o $(NAME)
else
	gcc $(FLAGS) -O3 -fsanitize=address $(SDL_HEADER) $(SDL_FRAMEWORKS) -F ./ $(FILES) $(LIB) -I $(INCLUDE) -o $(NAME) -rpath @executable_path
endif

clean:
#@make clean -C libft

fclean: clean
	rm -f $(NAME)
#@make fclean -C libft

re: fclean
	make all
