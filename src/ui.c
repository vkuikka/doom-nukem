/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 08:50:56 by rpehkone          #+#    #+#             */
/*   Updated: 2021/03/29 23:31:54 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

t_ui_state	*get_ui_state(t_ui_state *get_state)
{
	static t_ui_state *state = NULL;

	if (get_state)
		state = get_state;
	else
		return (state);
	return (NULL);
}

void		set_text_color(int color)
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	state->ui_text_color = color;
}

static SDL_Color get_text_color(void)
{
	SDL_Color	res;
	unsigned	get;
	t_ui_state	*state;

	state = get_ui_state(NULL);
	get = state->ui_text_color;
	res.r = (get << 8 * 0) >> 8 * 3;
	res.g = (get << 8 * 1) >> 8 * 3;
	res.b = (get << 8 * 2) >> 8 * 3;
	res.a = (get << 8 * 3) >> 8 * 3;
	return (res);
}

static void	put_text(char *text, t_window *window, SDL_Texture *texture, t_ivec2 pos)
{
	static TTF_Font*	font = NULL;

	if (!font)
	{
		font = TTF_OpenFont("Roboto-Medium.ttf", 13);
		if (!font)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			ft_error("font open fail");
		}
	}
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, get_text_color());
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
}

static void	ui_render_background(SDL_Renderer *SDLrenderer)
{
	static SDL_Texture *background = NULL;
	SDL_Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = 100;
	rect.h = 100;
	// if (!background)
		// background = SDL_CreateTexture(SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	// SDL_FillRect(screenSurface, &rect, SDL_MapRGB(...))
}

static void	ui_render_internal(SDL_Texture *get_text, SDL_Texture *get_streaming, t_window *get_window, t_ui_state *state)
{
	static SDL_Texture *text_texture;
	static SDL_Texture *streaming_texture;
	static t_window *window;
	unsigned	*pixels = NULL;
	signed		width;
	
	if (get_text)//on init
	{
		text_texture = get_text;
		streaming_texture = get_streaming;
		window = get_window;
		return ;
	}
	if (state)//add line of text
	{
		t_ivec2 text_pos;
		text_pos.x = state->ui_text_x_offset;
		text_pos.y = state->ui_text_y_pos;
		put_text(state->text, window, text_texture, text_pos);
		state->ui_text_y_pos += 14;
	}
	else//render
	{
		ui_render_background(window->SDLrenderer);
		SDL_UnlockTexture(streaming_texture);
		SDL_RenderCopy(window->SDLrenderer, streaming_texture, NULL, NULL);
		if (SDL_LockTexture(streaming_texture, NULL, (void**)&pixels, &width) != 0)
			ft_error("failed to lock texture\n");
		ft_memset(pixels, 0, RES_X * RES_Y * 4);
		SDL_RenderCopy(window->SDLrenderer, text_texture, NULL, NULL);

		//maybe this on windows
		// SDL_SetRenderTarget(window->SDLrenderer, text_texture);
		// SDL_RenderClear(window->SDLrenderer);
		// SDL_SetRenderTarget(window->SDLrenderer, NULL);
	}
}

static void	edit_slider_var(float *unit, int yloc)
{
	static int	drag[1000];
	int			x;
	int			y;

	if (!SDL_GetRelativeMouseMode() && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (y >= yloc + 4 && y <= yloc + 15 && x < 109 && x > 2)
			*unit = (float)(x - 4) / (float)100;
	}
}

static void	edit_button_var(int *var, int yloc)
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

static void	button_pixel_put(int x, int y, int color, unsigned *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return;
	texture[x + (y * RES_X)] = color;
}

static void	render_button_streaming(unsigned *get_texture, int *var, int dy)
{
	static unsigned *texture;

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
	edit_button_var(var, dy);
}

static void	render_slider_streaming(unsigned *get_texture, float unit, int dy)
{
	static unsigned *texture;

	if (get_texture)
	{
		texture = get_texture;
		return ;
	}
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 106; x++)
		{
			button_pixel_put(x + 2, y + 6 + dy, 0x404040ff, texture);
		}
	}
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 6; x++)
		{
			button_pixel_put(x + 2 + (100 * unit), y + 2 + dy, 0x666666ff, texture);
		}
	}
}

void	text(char *text)
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	state->text = text;
	state->ui_text_x_offset = 4;
	ui_render_internal(NULL, NULL, NULL, state);
}

void	button(int *var, char *text)
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	state->text = text;
	state->ui_text_x_offset = 14;
	render_button_streaming(NULL, var, state->ui_text_y_pos);
	ui_render_internal(NULL, NULL, NULL, state);
}

float	clamp(float var, float min, float max)
{
	if (var < min)
		var = min;
	else if (var > max)
		var = max;
	return (var);
}

void	int_slider(int *var, char *str, int min, int max)
{
	t_ui_state	*state;
	float		unit;

	text(str);
	state = get_ui_state(NULL);
	state->text = "";
	state->ui_text_x_offset = 14;

	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider_streaming(NULL, unit, state->ui_text_y_pos);
	edit_slider_var(&unit, state->ui_text_y_pos);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += 14;
}

void	ui_render(t_level *level)
{
	t_ui_state	state;

	get_ui_state(&state);
	ui_config(level);
	ui_render_internal(NULL, NULL, NULL, NULL);
}

void	init_ui(t_window *window, t_level *level)
{
	//maybe this on windows
	// SDL_Texture *text_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_X, RES_Y);
	SDL_Texture *text_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	SDL_Texture *ui_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	unsigned	*pixels;
	signed		width;
	t_editor_ui *ui;

	ui = (t_editor_ui*)malloc(sizeof(t_editor_ui));
	level->ui = ui;
	ft_bzero(ui, sizeof(t_editor_ui));
	ui->backface_culling = 1;
	ui->distance_culling = 1;
	ui->render_distance = 20;
	ui->raycast_quality = NOISE_QUALITY_LIMIT - 1;
	// ui->fog_color = 0xffffffff;//fog
	// ui->fog_color = 0x000000ff;//night
	// ui->fog_color = 0xff0000ff;
	ui->fog_color = 0xb19a6aff;//sandstorm
	// ui->fog_color = 0xddddddff;//smoke

	TTF_Init();
	SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(ui_texture, SDL_BLENDMODE_BLEND);
	if (SDL_LockTexture(ui_texture, NULL, (void**)&pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	ui_render_internal(text_texture, ui_texture, window, NULL);
	render_button_streaming(pixels, 0, 0);
	render_slider_streaming(pixels, 0, 0);
 }
