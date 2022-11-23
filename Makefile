# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2022/11/11 15:31:34 by rpehkone         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = doom-nukem
SRCDIR = src/
SRC = main.c box_blur.c radial_gradient.c ssao.c render_raycast.c input.c game_logic.c color.c culling.c culling_occlusion.c vectors.c init.c file_read.c raycast.c obj_read.c bmp_read.c find_quads.c wireframe.c editor_set_state.c physics.c enemies.c enemy_state_machine.c shaders.c select.c screen_space_partition.c uv_overlap.c 2d_intersect.c obj_editor.c door_runtime.c bake.c spray.c audio.c cast_face.c camera_path.c shader_perlin.c bloom.c animation.c sprite.c projectile.c dynamic_geometry.c filesystem_macos.c embed_macos.c
SRC_UI = color_hsl.c door_editor.c distortion.c gizmo.c hud.c light_editor.c main_menu.c nonfatal_error.c ui_config.c ui_elements.c ui.c uv_editor.c game_logic_editor.c
SRC_SERIALIZATION = read_write.c serialize.c deserialize.c serialize_primitive.c deserialize_primitive.c
UI_FILES=$(addprefix ui/, $(SRC_UI))
SERIALIZATION_FILES=$(addprefix serialization/, $(SRC_SERIALIZATION))
SRC += $(UI_FILES)
SRC += $(SERIALIZATION_FILES)
FILES=$(addprefix src/, $(SRC))

AUDIO_EFFECT = death.ogg doorsliding.wav gunshot.wav jump.wav osrsMonsterDeath.wav teleport.wav windowShatter.wav door.wav gettinghit.wav jetpack.wav osrsDeath.wav reload.wav victory.wav
MUSIC = ingame.ogg main_menu.ogg
AUDIO_FILES=$(addprefix music/, $(MUSIC))
AUDIO_FILES+=$(addprefix effects/, $(AUDIO_EFFECT))
AUDIO=$(addprefix audio/, $(AUDIO_FILES))
EMBED_FILES = title.bmp skybox.obj digital.ttf roboto.ttf ammo_diff.bmp health_diff.bmp pickup_box.obj enemy_diff.bmp lightsprite.bmp skybox.bmp spray.bmp projectile.bmp enemy_shoot.obj
EMBED_LEVEL_FILES = ship_final.obj out.bmp normal.bmp
EMBED_LEVEL=$(addprefix level/, $(EMBED_LEVEL_FILES))
EMBED_VIEWMODEL_FILES = viewmodel_000000.obj viewmodel_000001.obj viewmodel_000002.obj viewmodel_000003.obj viewmodel_000004.obj viewmodel_000005.obj viewmodel_000006.obj viewmodel_000007.obj viewmodel_000008.obj viewmodel_000009.obj viewmodel_000010.obj vulcan_diff.bmp
EMBED_VIEWMODEL=$(addprefix viewmodel/, $(EMBED_VIEWMODEL_FILES))
EMBED_ANIM_RUN_FILES = enemy_run_000000.obj enemy_run_000002.obj enemy_run_000004.obj enemy_run_000006.obj enemy_run_000008.obj enemy_run_000001.obj enemy_run_000003.obj enemy_run_000005.obj enemy_run_000007.obj enemy_run_000009.obj
EMBED_ANIM_DIE_FILES = enemy_die_000000.obj enemy_die_000002.obj enemy_die_000004.obj enemy_die_000006.obj enemy_die_000008.obj enemy_die_000010.obj enemy_die_000012.obj enemy_die_000001.obj enemy_die_000003.obj enemy_die_000005.obj enemy_die_000007.obj enemy_die_000009.obj enemy_die_000011.obj enemy_die_000013.obj
EMBED_ANIM_RUN=$(addprefix enemy_run/, $(EMBED_ANIM_RUN_FILES))
EMBED_ANIM_DIE=$(addprefix enemy_die/, $(EMBED_ANIM_DIE_FILES))
EMBED_FILES += $(EMBED_ANIM_RUN)
EMBED_FILES += $(EMBED_ANIM_DIE)
EMBED_FILES += $(EMBED_VIEWMODEL)
EMBED_FILES2 = $(EMBED_LEVEL)
EMBED_FILES2 += $(VIEWMODEL)
EMBED_FILES2 += $(AUDIO)
EMBED=$(addprefix embed/, $(EMBED_FILES))
EMBED2=$(addprefix embed/, $(EMBED_FILES2))

LIB = libft/libft.a libft/ft_printf/libftprintf.a
INCLUDE = -I libft/includes -I ./ -I src/ -I libft/ft_printf/
SDL_FRAMEWORKS = -framework SDL2 -framework SDL2_ttf -framework SDL2_mixer
SDL_HEADER = -I SDL2.framework/Headers
FLAGS = -Wall -Wextra -Werror -O3

.PHONY: clean fclean re all

all: $(NAME)

$(NAME):
	@make -C libft
	@touch embed.c
	@touch embed2.c
	@gcc -o embed.o -c embed.c
	@gcc -o embed2.o -c embed2.c
	@$(foreach file, $(EMBED), END=$$(echo $(file) | sed 's:.*/::' | tail -c 16); echo "$$END"; ld -r -o embed.o -sectcreate embed "$$END" $(file) embed.o;)
	@$(foreach file, $(EMBED2), END=$$(echo $(file) | sed 's:.*/::' | tail -c 16); echo "$$END"; ld -r -o embed2.o -sectcreate embed "$$END" $(file) embed2.o;)
	-gcc $(FLAGS) $(SDL_HEADER) $(SDL_FRAMEWORKS) -F ./ $(FILES) embed.o embed2.o $(LIB) $(INCLUDE) -o $(NAME) -rpath @executable_path
	@rm embed.o embed.c embed2.o embed2.c

clean:
	@make clean -C libft

fclean: clean
	@rm -f $(NAME)
	@make fclean -C libft

re: fclean
	@make all