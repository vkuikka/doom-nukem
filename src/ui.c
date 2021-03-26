/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buttons.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 08:50:56 by rpehkone          #+#    #+#             */
/*   Updated: 2021/02/10 03:42:47 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	button_pixel_put(int x, int y, int color, unsigned *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return;
	texture[x + (y * RES_X)] = color;
}

static int	get_ui_width(int x)
{
	static int	max_x = 0;
	int			tmp;

	if (!x)
	{
		tmp = max_x;
		max_x = 0;
		return (tmp);
	}
	if (x > max_x)
		max_x = x;
	return (0);
}

static void	put_text(char *text, t_window *window, SDL_Texture *texture)
{
	TTF_Font*	font;
	SDL_Color	color;

	font = TTF_OpenFont("Roboto-Medium.ttf", 13);
	if (!font)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		ft_error("font open fail");
	}
	color.r = 0xff;
	color.g = 0xff;
	color.b = 0xff;
	color.a = 0xff;
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(window->SDLrenderer, surfaceMessage);
	SDL_Rect text_rect;
	static int y = 0;
	text_rect.x = 14;
	text_rect.y = y;
	text_rect.w = 0;
	text_rect.h = 0;
	TTF_SizeText(font, text, &text_rect.w, &text_rect.h);
	y += text_rect.h - 2;
	SDL_SetRenderTarget(window->SDLrenderer, texture);
	SDL_RenderCopy(window->SDLrenderer, Message, NULL, &text_rect);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	get_ui_width(text_rect.x);
}

static void	print_ui_background(SDL_Renderer *SDLrenderer)
{
	static SDL_Texture *background = NULL;
	SDL_Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = get_ui_width(0);
	rect.h = 100;
	// if (!background)
		// background = SDL_CreateTexture(SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	// SDL_FillRect(screenSurface, &rect, SDL_MapRGB(...))
}

static void	draw_buttons_internal(SDL_Texture *get_text, SDL_Texture *get_button, t_window *get_window, char *text)
{
	static int	first = 1;
	static SDL_Texture *text_texture;
	static SDL_Texture *button_texture;
	static t_window *window;
	unsigned	*pixels = NULL;
	signed		width;
	
	if (get_text)
	{
		text_texture = get_text;
		button_texture = get_button;
		window = get_window;
		return ;
	}
	if (!text)
	{
		print_ui_background(window->SDLrenderer);
		SDL_UnlockTexture(button_texture);
		SDL_RenderCopy(window->SDLrenderer, button_texture, NULL, NULL);
		if (SDL_LockTexture(button_texture, NULL, (void**)&pixels, &width) != 0)
			ft_error("failed to lock texture\n");
		//ft_memset(pixels, 0, RES_X * RES_Y * 4);
		first = 0;
	}
	if (text && first)
	{
		put_text(text, window, text_texture);
	}
	else
		SDL_RenderCopy(window->SDLrenderer, text_texture, NULL, NULL);
}

static void	edit_var(int *var, int yloc)
{
	static int	last[1000];
	int			x;
	int			y;

	if (!SDL_GetRelativeMouseMode() && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (last[yloc] == 0 && x >= 2 && x <= 11 && y >= yloc && y <= yloc + 14)
			*var = *var ? 0 : 1;
		last[yloc] = 1;
	}
	else
		last[yloc] = 0;
}

static void	add_button(unsigned *get_texture, int *var, char *text, int clear)
{
	static unsigned *texture;

	static int dy = 0;
	if (clear == 42)
	{
		dy += 14;
		return ;
	}
	else if (clear)
	{
		dy = 0;
		return ;
	}
	if (get_texture)
	{
		texture = get_texture;
		return ;
	}
	int color = *var ? 0x008020ff : 0x303030ff;
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (y < 1 || y > 8 || x < 1 || x > 8)
				button_pixel_put(x + 2, y + 4 + dy, 0x404040ff, texture);
			else
				button_pixel_put(x + 2, y + 4 + dy, color, texture);
		}
	}
	edit_var(var, dy);
	dy += 14;
}

void	text(char *text)
{
	add_button(NULL, NULL, NULL, 42);
	draw_buttons_internal(NULL, NULL, NULL, text);
}

void	button(int *var, char *text)
{
	add_button(NULL, var, text, 0);
	draw_buttons_internal(NULL, NULL, NULL, text);
}

void	int_slider(int *var, char *str, int min, int max)
{
	text(str);
	// clamp(var, min, max);
}

void	add_button_text(t_editor_ui *get_buttons)
{
	static t_editor_ui *buttons = NULL;

	if (get_buttons)
		buttons = get_buttons;
	if (get_buttons)
		return ;
	//char[100] render_scale_text;
	// sprintf(&render_scale_text, "render scale 1/%d", (1 / quality) * 100)
	// int_slider(&render_scale_text, "render scale", 1, 20);1/n .. 1/7%
	button(&buttons->noclip, "noclip");
	button(&buttons->wireframe, "wireframe");
	button(&buttons->show_quads, "quad visualize");
	// button(, "face/vert selection");
	// button(&buttons->show_quads, "normals");
	text(" ");
	text("Level:");
	button(&buttons->fog, "fog");
	// color(buttons->color, "fog color");
	// button(, "set spawn point");
	text("Culling:");
	// button(&buttons->pause_culling_position, "\tpause");
	button(&buttons->backface_culling, "\tbackface");
	button(&buttons->distance_culling, "\tdistance");
	// slider(buttons->distance_culling_distance, "\tz far clip distance", 2, 50);
	// text("Selected face:");
	// slider("transparency", 0, 1);
	// button("flip normal");
	// button("\tforce disable culling");
	// button("\tmirror");
	// button("\tinfinite");
	//oisko jos hover n'it' hightlightaa facet miss' se on p''ll'
}

void	draw_buttons(void)
{
	add_button_text(NULL);
	draw_buttons_internal(NULL, NULL, NULL, NULL);
	add_button(NULL, 0, NULL, 1);
}

void	init_buttons(t_window *window, t_editor_ui *buttons)
{
	SDL_Texture *text_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_X, RES_Y);
	SDL_Texture *button_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	unsigned	*pixels;
	signed		width;
	
	ft_bzero(buttons, sizeof(t_editor_ui));
	buttons->backface_culling = 1;
	buttons->distance_culling = 1;
	TTF_Init();
	SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(button_texture, SDL_BLENDMODE_BLEND);
	if (SDL_LockTexture(button_texture, NULL, (void**)&pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	draw_buttons_internal(text_texture, button_texture, window, NULL);
	add_button(pixels, 0, NULL, 0); 
	add_button_text(buttons);
 }
