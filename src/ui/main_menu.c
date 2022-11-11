/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_menu.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 13:52:25 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	main_menu_text_background(t_rect rect, unsigned int *pixels)
{
	int	x;
	int	y;

	x = rect.x;
	while (x < rect.x + rect.w)
	{
		y = rect.y;
		while (y < rect.y + rect.h)
		{
			if (x >= 0 && y >= 0 && x < RES_X && y < RES_Y)
				pixels[x + (y * RES_X)] = MAIN_MENU_FONT_BACKGROUND_COLOR;
			y++;
		}
		x++;
	}
}

void	main_menu_title(t_bmp *img, unsigned int *pixels)
{
	static float	amount = 0;

	amount += .02;
	fake_analog_signal(img, pixels, sinf(amount) / 2 + 0.5);
}

int	mouse_collision(t_rect rect, t_ivec2 mouse)
{
	if (mouse.x >= rect.x && mouse.x < rect.x + rect.w
		&& mouse.y >= rect.y && mouse.y < rect.y + rect.h)
		return (TRUE);
	return (FALSE);
}

int	main_menu_button_text(char *text, int index,
					t_level *level, unsigned int *pixels)
{
	t_rect	rect;
	t_ivec2	res;

	rect.x = MAIN_MENU_FONT_SIZE * 2;
	rect.y = RES_Y / 2
		+ ((MAIN_MENU_FONT_SIZE * MAIN_MENU_FONT_PADDING_MULTIPLIER)
			* (index - (MAIN_MENU_BUTTON_AMOUNT / 2)));
	res = render_text(text, rect.x, rect.y);
	rect.w = res.x;
	rect.h = res.y;
	rect.x -= MAIN_MENU_FONT_SIZE / 4;
	rect.w += MAIN_MENU_FONT_SIZE / 2;
	if (mouse_collision(rect, level->ui.state.mouse))
	{
		main_menu_text_background(rect, pixels);
		if (level->ui.state.m1_click)
			return (TRUE);
	}
	return (FALSE);
}

void	fix_faces(t_level *level)
{
	int	i;

	i = 0;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].isbroken)
			level->all.tris[i].isbroken = FALSE;
		i++;
	}
}

void	main_menu_buttons_level(t_game_state *game_state, int *state_changed,
						t_level *level, unsigned int *pixels)
{
	if (main_menu_button_text("play", 0, level, pixels)
		&& level->bake_status != BAKE_BAKING)
	{
		*game_state = GAME_STATE_INGAME;
		*state_changed = TRUE;
		level->ui.noclip = FALSE;
		level->game_logic.player.health = PLAYER_HEALTH_MAX;
		level->game_logic.player.ammo = PLAYER_AMMO_MAX;
		spawn_enemies(level);
		fix_faces(level);
		Mix_PlayMusic(level->audio.game_music, -1);
	}
	if (main_menu_button_text("edit level", 1, level, pixels))
	{
		Mix_HaltMusic();
		level->ui.state.m1_click = FALSE;
		level->ui.noclip = TRUE;
		*game_state = GAME_STATE_EDITOR;
		*state_changed = TRUE;
		level->ui.state.ui_location = UI_LOCATION_MAIN;
		fix_faces(level);
	}
}

void	main_menu_buttons_other(t_game_state *game_state, int *state_changed,
						t_level *level, unsigned int *pixels)
{
	if (main_menu_button_text("select level", 2, level, pixels))
	{
		level->ui.state.ui_location = UI_LOCATION_FILE_OPEN;
		level->ui.main_menu = MAIN_MENU_LOCATION_LEVEL_SELECT;
		ft_strcpy(level->ui.state.extension, ".doom-nukem");
		level->ui.state.open_file = &open_level;
	}
	if (main_menu_button_text("create level", 3, level, pixels))
	{
		Mix_HaltMusic();
		create_default_level(level);
		level->ui.noclip = TRUE;
		*game_state = GAME_STATE_EDITOR;
		*state_changed = TRUE;
		level->level_initialized = TRUE;
	}
	if (main_menu_button_text("settings", 4, level, pixels))
		level->ui.main_menu = MAIN_MENU_LOCATION_SETTINGS;
}

void	main_menu(t_level *level, unsigned int *pixels,
			t_game_state *game_state)
{
	int	state_changed;

	level->ui.state.current_font = level->ui.main_menu_font;
	set_text_color(MAIN_MENU_FONT_COLOR);
	main_menu_title(&level->main_menu_title, pixels);
	state_changed = FALSE;
	level->ui.state.ui_max_width = 0;
	if (level->level_initialized)
		main_menu_buttons_level(game_state, &state_changed, level, pixels);
	main_menu_buttons_other(game_state, &state_changed, level, pixels);
	if (state_changed)
	{
		level->cam.pos = level->game_logic.spawn_pos.pos;
		level->cam.look_side = level->game_logic.spawn_pos.look_side;
		level->cam.look_up = level->game_logic.spawn_pos.look_up;
		level->game_logic.player.vel.x = 0;
		level->game_logic.player.vel.y = 0;
		level->game_logic.player.vel.z = 0;
	}
}
