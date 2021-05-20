/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_menu.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/19 18:51:47 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	put_text_main_menu(char *text, t_window *window, SDL_Texture *texture, t_rect *pos)
{
	static TTF_Font*	font = NULL;

	if (!font)
	{
		font = TTF_OpenFont("Roboto-Medium.ttf", MAIN_MENU_FONT_SIZE);
		if (!font)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			ft_error("font open fail");
		}
	}
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, get_sdl_color(MAIN_MENU_FONT_COLOR));
	SDL_Texture* Message = SDL_CreateTextureFromSurface(window->SDLrenderer, surfaceMessage);
	SDL_Rect text_rect;
	text_rect.w = 0;
	text_rect.h = 0;
	TTF_SizeText(font, text, &text_rect.w, &text_rect.h);
	text_rect.x = pos->x;
	text_rect.y = pos->y;
	SDL_SetRenderTarget(window->SDLrenderer, texture);
	SDL_RenderCopy(window->SDLrenderer, Message, NULL, &text_rect);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	pos->w = text_rect.w;
	pos->h = text_rect.h;
}

static t_rect	main_menu_button_text(char *text, t_window *window, SDL_Texture *texture, int index)
{
	t_rect		rect;

	rect.x = MAIN_MENU_FONT_SIZE * 2;
	rect.y = RES_Y / 2 + ((MAIN_MENU_FONT_SIZE * MAIN_MENU_FONT_PADDING_MULTIPLIER) * (index - (MAIN_MENU_BUTTON_AMOUNT / 2)));
	put_text_main_menu(text, window, texture, &rect);
	rect.x -= MAIN_MENU_FONT_SIZE / 4;
	rect.w += MAIN_MENU_FONT_SIZE / 2;
	return (rect);
}

static void main_menu_text_background(t_rect rect, unsigned *pixels)
{
	for (int x = rect.x; x < rect.x + rect.w; x++)
		for (int y = rect.y; y < rect.y + rect.h; y++)
			if (x >= 0 && y >= 0 && x < RES_X && y < RES_Y)
				pixels[x + (y * RES_X)] = MAIN_MENU_FONT_BACKGROUND_COLOR;
}

static int		mouse_collision(t_rect rect, t_level *level)
{
	t_ivec2	mouse;

	if (level->ui.state.m1_click)
	{
		SDL_GetMouseState(&mouse.x, &mouse.y);
		if (mouse.x >= rect.x && mouse.x < rect.x + rect.w &&
			mouse.y >= rect.y && mouse.y < rect.y + rect.h)
			return (1);
	}
	return (0);
}

void			main_menu_move_background(t_level *level)
{
	//lerp pos1 pos2
}

void			main_menu(t_level *level, t_window *window, t_game_state *game_state)
{
	static SDL_Texture *texture = NULL;
	static SDL_Texture *background_texture = NULL;
	unsigned	*pixels;
	signed		width;
	t_rect		rect;

	if (!texture)
	{
		texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_X, RES_Y);
		background_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(background_texture, SDL_BLENDMODE_BLEND);
	}
	if (SDL_LockTexture(background_texture, NULL, (void**)&pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	ft_memset(pixels, 0, RES_X * RES_Y * 4);
	rect = main_menu_button_text("play level", window, texture, 0);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_INGAME;
	}
	rect = main_menu_button_text("edit level", window, texture, 1);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_EDITOR;
	}
	rect = main_menu_button_text("new level", window, texture, 2);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_EDITOR;
	}
	rect = main_menu_button_text("settings", window, texture, 3);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_EDITOR;
		level->ui.state.ui_location = UI_LOCATION_SETTINGS;
	}
	SDL_UnlockTexture(background_texture);
	SDL_RenderCopy(window->SDLrenderer, background_texture, NULL, NULL);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
	SDL_SetRenderTarget(window->SDLrenderer, texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderPresent(window->SDLrenderer);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
}
