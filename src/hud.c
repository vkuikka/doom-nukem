/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hud.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:48:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/19 18:48:10 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

t_ivec2			put_text_hud(char *text, t_window *window, SDL_Texture *texture, t_ivec2 pos)
{
	static TTF_Font*	font = NULL;

	if (!font)
	{
		font = TTF_OpenFont("embed/digital.ttf", HUD_FONT_SIZE);
		if (!font)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			ft_error("font open fail");
		}
	}
	// SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, get_text_color());
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, get_sdl_color(HUD_TEXT_COLOR));
	SDL_Texture* Message = SDL_CreateTextureFromSurface(window->SDLrenderer, surfaceMessage);
	SDL_Rect text_rect;
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
	t_ivec2 size;
	size.x = text_rect.w;
	size.y = text_rect.h;
	return (size);
}

static void	pixel_put_hud(int x, int y, int color, unsigned *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return;
	texture[x + (y * RES_X)] = color;
}

static void	crosshair(unsigned *pixels)
{
	int x;
	int y;
	int i;
	int j;

	x = RES_X / 2;
	y = RES_Y / 2;
	i = -1;
	while (++i < 10 && (j = -1))
		while (++j < 3)
		{
			pixel_put_hud(x + i - 14, y + j, CROSSHAIR_COLOR, pixels);
			pixel_put_hud(x + i + 4, y + j, CROSSHAIR_COLOR, pixels);
		}
	i = -1;
	while (++i < 3 && (j = -1))
		while (++j < 10)
		{
			pixel_put_hud(x + i - 1, y + j - 14, CROSSHAIR_COLOR, pixels);
			pixel_put_hud(x + i - 1, y + j + 6, CROSSHAIR_COLOR, pixels);
		}
}

static void	viewmodel(unsigned *pixels, t_bmp img)
{
	for (int x = 0; x < img.width; x++)
	{
		for (int y = 0; y < img.height; y++)
		{
			pixel_put_hud(RES_X - img.width + x - 40, RES_Y - img.height + y, img.image[x + (y * img.width)], pixels);
		}
	}
}

void		hud(t_level *level, t_window *window)
{
	static SDL_Texture *texture = NULL;
	unsigned	*pixels;
	signed		width;
	char		buf[100];

	if (!texture)
	{
		texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	}
	if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	ft_memset(pixels, 0, RES_X * RES_Y * 4);
	viewmodel(pixels, level->viewmodel[level->viewmodel_index]);
	crosshair(pixels);
	sprintf(buf, "%d+", level->player_health);
	put_text_hud(buf, window, texture, (t_ivec2){HUD_FONT_SIZE / 4, RES_Y - HUD_FONT_SIZE});
	sprintf(buf, "%d", level->player_ammo);
	put_text_hud(buf, window, texture, (t_ivec2){RES_X - ((HUD_FONT_SIZE / 2) * strlen(buf)), RES_Y - HUD_FONT_SIZE});
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
}