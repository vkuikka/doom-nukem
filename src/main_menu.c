/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_menu.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 18:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/17 23:58:35 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	put_text_main_menu(char *text, t_window *window, SDL_Texture *texture, t_rect *pos)
{
	static TTF_Font*	font = NULL;

	if (!font)
	{
		font = TTF_OpenFont("embed/Roboto-Medium.ttf", MAIN_MENU_FONT_SIZE);
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

static void main_menu_title(t_bmp *img, unsigned *pixels)
{
	for (int y = 0; y < RES_Y && y < img->height; y++)
		for (int x = 0; x < RES_X; x++)
			if (x >= 0 && y >= 0 && x < RES_X && y < RES_Y)
				pixels[x + (y * RES_X)] = img->image[(int)(((float)x / RES_X) * img->width) + (y * img->width)];
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
	float	time;

	time = (SDL_GetTicks() - level->main_menu_anim_start_time) / (1000.0 * level->main_menu_anim_time);
	if (time < 1)
	{
		level->cam.pos = vec_interpolate(level->main_menu_pos1.pos, level->main_menu_pos2.pos, time);
		level->cam.look_side = lerp(level->main_menu_pos1.look_side, level->main_menu_pos2.look_side, time);
		level->cam.look_up = lerp(level->main_menu_pos1.look_up, level->main_menu_pos2.look_up, time);
	}
	else if (time < 2)
	{
		time -= 1;
		level->cam.pos = vec_interpolate(level->main_menu_pos2.pos, level->main_menu_pos1.pos, time);
		level->cam.look_side = lerp(level->main_menu_pos2.look_side, level->main_menu_pos1.look_side, time);
		level->cam.look_up = lerp(level->main_menu_pos2.look_up, level->main_menu_pos1.look_up, time);
	}
	else
		level->main_menu_anim_start_time = SDL_GetTicks();
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
	main_menu_title(&level->main_menu_title, pixels);
	rect = main_menu_button_text("play level", window, texture, 0);
	main_menu_text_background(rect, pixels);
	int state_changed = FALSE;
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_INGAME;
		state_changed = TRUE;
		level->player_health = PLAYER_HEALTH_MAX;
		level->player_ammo = PLAYER_AMMO_MAX;
	}
	rect = main_menu_button_text("edit level", window, texture, 1);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_EDITOR;
		state_changed = TRUE;
	}
	rect = main_menu_button_text("new level", window, texture, 2);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_EDITOR;
		state_changed = TRUE;
	}
	rect = main_menu_button_text("settings", window, texture, 3);
	main_menu_text_background(rect, pixels);
	if (mouse_collision(rect, level))
	{
		*game_state = GAME_STATE_EDITOR;
		level->ui.state.ui_location = UI_LOCATION_SETTINGS;
		state_changed = TRUE;
	}
	if (state_changed)
	{
		level->cam.pos = level->spawn_pos.pos;
		level->cam.look_side = level->spawn_pos.look_side;
		level->cam.look_up = level->spawn_pos.look_up;
		level->player_vel.x = 0;
		level->player_vel.y = 0;
		level->player_vel.z = 0;
	}
	SDL_UnlockTexture(background_texture);
	SDL_RenderCopy(window->SDLrenderer, background_texture, NULL, NULL);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
	SDL_SetRenderTarget(window->SDLrenderer, texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderPresent(window->SDLrenderer);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
}
