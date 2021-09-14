/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_menu.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/02 19:18:11 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	main_menu_text_background(t_rect rect, unsigned int *pixels)
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

static void	main_menu_title(t_bmp *img, unsigned int *pixels)
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

static int	main_menu_button_text(char *text, int index,
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

void	main_menu_move_background(t_level *level)
{
	float	time;

	time = (SDL_GetTicks() - level->main_menu_anim_start_time)
		/ (1000.0 * level->main_menu_anim_time);
	if (time < 1)
	{
		level->cam.pos = vec_interpolate(level->main_menu_pos1.pos,
				level->main_menu_pos2.pos, time);
		level->cam.look_side = lerp(level->main_menu_pos1.look_side,
				level->main_menu_pos2.look_side, time);
		level->cam.look_up = lerp(level->main_menu_pos1.look_up,
				level->main_menu_pos2.look_up, time);
	}
	else if (time < 2)
	{
		time -= 1;
		level->cam.pos = vec_interpolate(level->main_menu_pos2.pos,
				level->main_menu_pos1.pos, time);
		level->cam.look_side = lerp(level->main_menu_pos2.look_side,
				level->main_menu_pos1.look_side, time);
		level->cam.look_up = lerp(level->main_menu_pos2.look_up,
				level->main_menu_pos1.look_up, time);
	}
	else
		level->main_menu_anim_start_time = SDL_GetTicks();
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
		level->player_health = PLAYER_HEALTH_MAX;
		level->player_ammo = PLAYER_AMMO_MAX;
		Mix_PlayMusic(level->audio.game_music, -1);
	}
	if (main_menu_button_text("edit level", 1, level, pixels))
	{
		level->ui.state.m1_click = FALSE;
		level->ui.noclip = TRUE;
		*game_state = GAME_STATE_EDITOR;
		*state_changed = TRUE;
		level->ui.state.ui_location = UI_LOCATION_MAIN;
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
		level->cam.pos = level->spawn_pos.pos;
		level->cam.look_side = level->spawn_pos.look_side;
		level->cam.look_up = level->spawn_pos.look_up;
		level->player_vel.x = 0;
		level->player_vel.y = 0;
		level->player_vel.z = 0;
	}
}
