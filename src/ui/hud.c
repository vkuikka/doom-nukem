/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:48:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/20 23:11:52 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

unsigned int	cycle_rgb(unsigned int time)
{
	unsigned int	red;
	unsigned int	grn;
	unsigned int	blu;

	red = 0xff * ((sin((time / (CYCLE_RGB_LOOP_FPS * 1000.0))
					+ (M_PI / 1.5 * 0)) + 1) / 2);
	grn = 0xff * ((sin((time / (CYCLE_RGB_LOOP_FPS * 1000.0))
					+ (M_PI / 1.5 * 1)) + 1) / 2);
	blu = 0xff * ((sin((time / (CYCLE_RGB_LOOP_FPS * 1000.0))
					+ (M_PI / 1.5 * 2)) + 1) / 2);
	red = red << 8 * 3;
	grn = grn << 8 * 2;
	blu = blu << 8 * 1;
	return (red + grn + blu + 0x70);
}

static void	pixel_put_hud(int x, int y, int color, unsigned int *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return ;
	texture[x + (y * RES_X)] = color;
}

static void	death_overlay(unsigned int *pixels)
{
	int	x;
	int	y;

	x = 0;
	while (x < RES_X)
	{
		y = 0;
		while (y < RES_Y)
		{
			pixel_put_hud(x, y, DEATH_OVERLAY_COLOR, pixels);
			y++;
		}
		x++;
	}
}

static void	crosshair(unsigned int *pixels, int x, int y)
{
	int	i;
	int	j;

	i = -1;
	while (++i < 10)
	{
		j = -1;
		while (++j < 3)
		{
			pixel_put_hud(x + i - 14, y + j, CROSSHAIR_COLOR, pixels);
			pixel_put_hud(x + i + 4, y + j, CROSSHAIR_COLOR, pixels);
		}
	}
	i = -1;
	while (++i < 3)
	{
		j = -1;
		while (++j < 10)
		{
			pixel_put_hud(x + i - 1, y + j - 14, CROSSHAIR_COLOR, pixels);
			pixel_put_hud(x + i - 1, y + j + 6, CROSSHAIR_COLOR, pixels);
		}
	}
}

static void	viewmodel(unsigned int *pixels, t_bmp img)
{
	int	x;
	int	y;

	x = 25;
	while (x <= img.width)
	{
		y = 0;
		while (y < img.height)
		{
			pixel_put_hud(RES_X - img.width + x, RES_Y - img.height + y,
				img.image[x + (y * img.width)], pixels);
			y++;
		}
		x++;
	}
}

void	ingame_hud(t_level *level, unsigned int *pixels)
{
	char	buf[100];

	level->ui.state.current_font = level->ui.hud_font;
	set_text_color(HUD_TEXT_COLOR);
	crosshair(pixels, RES_X / 2, RES_Y / 2);
	sprintf(buf, "%d+", level->player_health);
	render_text(buf, HUD_FONT_SIZE / 4, RES_Y - HUD_FONT_SIZE);
	sprintf(buf, "%d", level->player_ammo);
	render_text(buf, RES_X - ((HUD_FONT_SIZE / 2) * strlen(buf)),
		RES_Y - HUD_FONT_SIZE);
}

void	hud(t_level *level, unsigned int *pixels, t_game_state game_state)
{
	int		width;

	viewmodel(pixels, level->viewmodel[level->viewmodel_index]);
	if (game_state == GAME_STATE_DEAD)
	{
		level->ui.state.current_font = level->ui.win_lose_font;
		death_overlay(pixels);
		set_text_color(HUD_GAME_EVENT_TEXT_COLOR);
		render_text("DEAD",
			RES_X / 2 - HUD_GAME_EVENT_FONT_SIZE,
			RES_Y / 2 - HUD_GAME_EVENT_FONT_SIZE / 2);
	}
	else if (game_state == GAME_STATE_WIN)
	{
		level->ui.state.current_font = level->ui.win_lose_font;
		set_text_color(cycle_rgb(SDL_GetTicks()));
		render_text("YOU WIN",
			RES_X / 2 - HUD_GAME_EVENT_FONT_SIZE * 2,
			RES_Y / 2 - HUD_GAME_EVENT_FONT_SIZE / 2);
	}
	else
		ingame_hud(level, pixels);
}
