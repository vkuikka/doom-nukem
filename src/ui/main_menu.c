/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_menu.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/24 23:33:28 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static t_rect	main_menu_button_text(char *text, int index)
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
	return (rect);
}

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

static unsigned int	black_and_white(unsigned int color, float amount)
{
	unsigned char	*rgb;
	unsigned int	res;

	amount = 1.0;
	rgb = (unsigned char *)&color;
	res = 0;
	res += rgb[3];
	res += rgb[2];
	res += rgb[1];
	res /= 3;
	rgb[3] = res * amount + rgb[3] * (1 - amount);
	rgb[2] = res * amount + rgb[2] * (1 - amount);
	rgb[1] = res * amount + rgb[1] * (1 - amount);
	return ((unsigned int)*(int *)rgb);
}

static unsigned int	chroma(t_bmp *img, int x, int y, int x_amount)
{
	unsigned int	res_color;
	unsigned char	*rgb_l;
	unsigned char	*rgb;
	unsigned char	*rgb_r;
	unsigned char	*res;

	rgb = (unsigned char *)&img->image[x + y * img->width];
	rgb_l = (unsigned char *)&img->image[x + y * img->width];
	rgb_r = (unsigned char *)&img->image[x + y * img->width];
	if (x_amount < 0)
		x_amount *= -1;
	if (x - x_amount > 0)
		rgb_l = (unsigned char *)&img->image[(x - x_amount) + y * img->width];
	if (x + x_amount < img->width)
		rgb_r = (unsigned char *)&img->image[(x + x_amount) + y * img->width];
	res = (unsigned char *)&res_color;
	res[3] = rgb_l[3];
	res[2] = rgb[2];
	res[1] = rgb_r[1];
	res[0] = rgb[0];
	return (res_color);
}

static void	main_menu_title(t_bmp *img, unsigned int *pixels)
{
	static int		disturbance_y = 0;
	static float	x_amount = 0;
	unsigned int	screen_x;
	unsigned int	color;
	t_ivec3			i;

	x_amount += .02;
	disturbance_y++;
	disturbance_y = disturbance_y % img->height;
	i.y = -1;
	while (++i.y < img->height)
	{
		i.z = (rand() % (int)(3 + 6 * (1 + sinf(x_amount)))) - 6;
		if (i.y > disturbance_y - 5 && i.y < disturbance_y + 5)
			i.z = (rand() % 15) - 6;
		i.x = -1;
		while (++i.x < img->width)
		{
			screen_x = (RES_X / 2 - img->width / 2) + i.z + i.x;
			color = chroma(img, i.x, i.y, i.z + 2);
			if (disturbance_y < 6 && disturbance_y % 2)
				color = black_and_white(color, 1.0);
			pixels[screen_x + i.y * RES_X] = color;
		}
	}
}

static int	mouse_collision(t_rect rect, t_level *level, unsigned int *pixels)
{
	t_ivec2	mouse;

	SDL_GetMouseState(&mouse.x, &mouse.y);
	if (mouse.x >= rect.x && mouse.x < rect.x + rect.w
		&& mouse.y >= rect.y && mouse.y < rect.y + rect.h)
	{
		main_menu_text_background(rect, pixels);
		if (level->ui.state.m1_click)
			return (1);
	}
	return (0);
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
	t_rect	rect;

	rect = main_menu_button_text("play", 0);
	if (mouse_collision(rect, level, pixels)
		&& level->bake_status != BAKE_BAKING)
	{
		*game_state = GAME_STATE_INGAME;
		*state_changed = TRUE;
		level->player_health = PLAYER_HEALTH_MAX;
		level->player_ammo = PLAYER_AMMO_MAX;
	}
	rect = main_menu_button_text("edit level", 1);
	if (mouse_collision(rect, level, pixels))
	{
		level->ui.noclip = TRUE;
		*game_state = GAME_STATE_EDITOR;
		*state_changed = TRUE;
	}
}

void	main_menu_buttons_other(t_game_state *game_state, int *state_changed,
						t_level *level, unsigned int *pixels)
{
	t_rect	rect;

	rect = main_menu_button_text("select level", 2);
	if (mouse_collision(rect, level, pixels))
	{
		level->ui.main_menu = MAIN_MENU_LOCATION_LEVEL_SELECT;
		ft_strcpy(level->ui.state.extension, ".doom-nukem");
		level->ui.state.open_file = &open_level;
	}
	rect = main_menu_button_text("create level", 3);
	if (mouse_collision(rect, level, pixels))
	{
		init_level(level);
		level->ui.noclip = TRUE;
		*game_state = GAME_STATE_EDITOR;
		*state_changed = TRUE;
		level->level_initialized = TRUE;
	}
	rect = main_menu_button_text("settings", 4);
	if (mouse_collision(rect, level, pixels))
	{
		level->ui.main_menu = MAIN_MENU_LOCATION_SETTINGS;
	}
}

void	main_menu(t_level *level, unsigned int *pixels,
			t_game_state *game_state)
{
	int	state_changed;

	level->ui.state.current_font = level->ui.main_menu_font;
	set_text_color(MAIN_MENU_FONT_COLOR);
	main_menu_title(&level->main_menu_title, pixels);
	state_changed = FALSE;
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
