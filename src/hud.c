/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:48:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/10 21:31:46 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

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

t_ivec2	put_text_hud_game_event(char *text, t_window *window,
								SDL_Texture *texture, unsigned int color)
{
	static TTF_Font	*font = NULL;
	SDL_Rect		text_rect;
	t_ivec2			size;
	SDL_Surface		*surfaceMessage;
	SDL_Texture		*Message;

	if (!font)
	{
		font = TTF_OpenFont("embed/Roboto-Medium.ttf",
				HUD_GAME_EVENT_FONT_SIZE);
		if (!font)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			ft_error("font open fail");
		}
	}
	surfaceMessage = TTF_RenderText_Blended(font, text, get_sdl_color(color));
	Message = SDL_CreateTextureFromSurface(window->SDLrenderer, surfaceMessage);
	text_rect.w = 0;
	text_rect.h = 0;
	TTF_SizeText(font, text, &text_rect.w, &text_rect.h);
	text_rect.x = RES_X / 2 - (text_rect.w / 2);
	text_rect.y = RES_Y / 3 - (text_rect.h / 2);
	SDL_SetRenderTarget(window->SDLrenderer, texture);
	SDL_RenderCopy(window->SDLrenderer, Message, NULL, &text_rect);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	size.x = text_rect.w;
	size.y = text_rect.h;
	return (size);
}

t_ivec2	put_text_hud(char *text, t_window *window, SDL_Texture *texture,
																t_ivec2 pos)
{
	static TTF_Font	*font = NULL;
	SDL_Rect		text_rect;
	t_ivec2			size;
	SDL_Surface		*surfaceMessage;
	SDL_Texture		*Message;

	if (!font)
	{
		font = TTF_OpenFont("embed/digital.ttf", HUD_FONT_SIZE);
		if (!font)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			ft_error("font open fail");
		}
	}
	surfaceMessage
		= TTF_RenderText_Blended(font, text, get_sdl_color(HUD_TEXT_COLOR));
	Message
		= SDL_CreateTextureFromSurface(window->SDLrenderer, surfaceMessage);
	text_rect.w = 0;
	text_rect.h = 0;
	TTF_SizeText(font, text, &text_rect.w, &text_rect.h);
	text_rect.x = pos.x;
	text_rect.y = pos.y;
	SDL_SetRenderTarget(window->SDLrenderer, texture);
	SDL_RenderCopy(window->SDLrenderer, Message, NULL, &text_rect);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	size.x = text_rect.w;
	size.y = text_rect.h;
	return (size);
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

void	hud(t_level *level, t_window *window, t_game_state game_state)
{
	static SDL_Texture	*texture = NULL;
	unsigned int		*pixels;
	signed				width;
	char				buf[100];

	if (!texture)
	{
		texture = SDL_CreateTexture(window->SDLrenderer,
				SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
				RES_X, RES_Y);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	}
	if (SDL_LockTexture(texture, NULL, (void **)&pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	ft_memset(pixels, 0, RES_X * RES_Y * 4);
	viewmodel(pixels, level->viewmodel[level->viewmodel_index]);
	if (game_state == GAME_STATE_DEAD)
	{
		death_overlay(pixels);
		put_text_hud_game_event(
			"DEAD", window, texture, HUD_GAME_EVENT_TEXT_COLOR);
	}
	else if (game_state == GAME_STATE_WIN)
	{
		put_text_hud_game_event(
			"YOU WIN", window, texture, cycle_rgb(SDL_GetTicks()));
	}
	else
	{
		crosshair(pixels, RES_X / 2, RES_Y / 2);
		sprintf(buf, "%d+", level->player_health);
		put_text_hud(buf, window, texture,
			(t_ivec2){HUD_FONT_SIZE / 4, RES_Y - HUD_FONT_SIZE});
		sprintf(buf, "%d", level->player_ammo);
		put_text_hud(buf, window, texture,
			(t_ivec2){RES_X - ((HUD_FONT_SIZE / 2) * strlen(buf)),
			RES_Y - HUD_FONT_SIZE});
	}
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
}
