# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2021/09/21 10:51:49 by vkuikka          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = dnukem
SRCDIR = src/
SRC = main.c input.c game_logic.c color.c culling.c culling_occlusion.c vectors.c init.c file_read.c raycast.c obj_read.c bmp_read.c find_quads.c wireframe.c editor_set_state.c physics.c enemies.c shaders.c select.c serialize.c screen_space_partition.c uv_overlap.c 2d_intersect.c obj_editor.c door_runtime.c bake.c spray.c audio.c cast_face.c camera_path.c shader_perlin.c animation.c
SRC_UI = color_hsl.c door_editor.c distortion.c gizmo.c hud.c light_editor.c main_menu.c nonfatal_error.c ui_config.c ui_elements.c ui.c uv_editor.c game_logic_editor.c
LIB = libft/libft.a -L x86_64-w64-mingw32/lib/
INCLUDE = libft/includes -I ./ -I src/ -I x86_64-w64-mingw32/include/ -I x86_64-w64-mingw32/include/SDL2
AUDIO_EFFECT = d_death.ogg doorsliding.wav gunshot.wav jump.wav osrsMonsterDeath.wav teleport.wav windowShatter.wav door.wav gettinghit.wav jetpack.wav osrsDeath.wav reload.wav victory.wav
MUSIC = ingame.ogg main_menu.ogg
AUDIO_FILES=$(addprefix music/, $(MUSIC))
AUDIO_FILES+=$(addprefix effects/, $(AUDIO_EFFECT))
AUDIO=$(addprefix audio/, $(AUDIO_FILES))
#VIEWMODEL_FILES = ak_0.bmp
#VIEWMODEL=$(addprefix viewmodel/, $(VIEWMODEL_FILES))
EMBED_FILES = title.bmp skybox.obj digital.ttf Roboto-Medium.ttf ammo_pickup_texture.bmp health_pickup_texture.bmp pickup_box.obj
EMBED_FILES += $(VIEWMODEL)
EMBED_FILES += $(AUDIO)
EMBED=$(addprefix embed/, $(EMBED_FILES))
SDL_FRAMEWORKS = -framework SDL2 -framework SDL2_ttf -framework SDL2_mixer
SDL_HEADER = -I SDL2.framework/Headers
FLAGS = -Wall -Wextra -Werror

.PHONY: clean fclean re all
UI_FILES=$(addprefix ui/, $(SRC_UI))
SRC += $(UI_FILES)
FILES=$(addprefix src/, $(SRC))

all: $(NAME)

$(NAME):
#@make -C libft
ifeq ($(OS),Windows_NT)
	$(foreach file, $(EMBED), wsl bash -c "xxd -i $(file) >> embed.h"&)
	move embed.h src/embed.h
	-gcc $(FLAGS) -O3 $(FILES) src/filesystem_windows.c $(LIB) -I $(INCLUDE) -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lws2_32 -o $(NAME)
	del src\embed.h
else
	$(foreach file, $(EMBED), xxd -i $(file) >> embed.h;)
	-gcc $(FLAGS) -O3 -fsanitize=address $(SDL_HEADER) $(SDL_FRAMEWORKS) -F ./ $(FILES) src/filesystem_macos.c $(LIB) -I $(INCLUDE) -o $(NAME) -rpath @executable_path
	rm embed.h
endif

clean:
	rm -f embed.h
#@make clean -C libft

fclean: clean
	rm -f $(NAME)
#@make fclean -C libft

re: fclean
	make all
