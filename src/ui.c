/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 08:50:56 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/12 15:13:11 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		nonfatal_error(t_level *level, char *message)
{
	int i;

	level->ui.state.error_amount++;
	i = level->ui.state.error_amount;
	if (!(level->ui.state.error_start_time = (unsigned*)realloc(level->ui.state.error_start_time, sizeof(unsigned) * (i))))
		ft_error("memory allocation failed\n");
	level->ui.state.error_start_time[i - 1] = SDL_GetTicks();
	if (!(level->ui.state.error_message = (char**)realloc(level->ui.state.error_message, sizeof(char*) * (i))))
		ft_error("memory allocation failed\n");
	if (!(level->ui.state.error_message[i - 1] = (char*)malloc(sizeof(char) * ft_strlen(message) + 1)))
		ft_error("memory allocation failed\n");
	ft_strcpy(level->ui.state.error_message[i - 1], message);
}

static void	ui_remove_expired_nonfatal_errors(t_level *level)
{
	int i;

	i = 0;
	while (i < level->ui.state.error_amount)
	{
		if (level->ui.state.error_start_time[i] + 1000 * NONFATAL_ERROR_LIFETIME_SECONDS < SDL_GetTicks())
		{
			free(level->ui.state.error_message[i]);
			for (int k = i; k < level->ui.state.error_amount - 1; k++)
			{
				level->ui.state.error_message[k] = level->ui.state.error_message[k + 1];
				level->ui.state.error_start_time[k] = level->ui.state.error_start_time[k + 1];
			}
			level->ui.state.error_amount--;
			i = -1;
		}
		i++;
	}
	i = level->ui.state.error_amount;
	if (i)
	{
		level->ui.state.error_start_time = (unsigned*)realloc(level->ui.state.error_start_time, sizeof(unsigned) * i);
		level->ui.state.error_message = (char**)realloc(level->ui.state.error_message, sizeof(char*) * i);
	}
	else
	{
		free(level->ui.state.error_start_time);
		free(level->ui.state.error_message);
		level->ui.state.error_start_time = NULL;
		level->ui.state.error_message = NULL;
	}
}

static int ui_nonfatal_get_fade(t_level *level, int i)
{
	unsigned time;

	time = level->ui.state.error_start_time[i] + 1000 * NONFATAL_ERROR_LIFETIME_SECONDS - SDL_GetTicks();
	if (time < NONFATAL_ERROR_FADEOUT_TIME_MS)
		return (0xff * (time / (float)NONFATAL_ERROR_FADEOUT_TIME_MS));
	return (0xff);
}

static void	ui_render_nonfatal_errors(SDL_Texture *texture, t_window *window, t_level *level)
{
	t_ivec2	rect;
	int		i;

	if (!level->ui.state.error_amount)
		return ;
	ui_remove_expired_nonfatal_errors(level);
	if (!level->ui.state.error_amount)
		return ;
	rect.x = level->ui.state.ui_max_width + UI_PADDING_4 + UI_PADDING_4;
	rect.y = RES_Y / 2;
	i = 0;
	while (i < level->ui.state.error_amount)
	{
		set_text_color(UI_ERROR_COLOR + ui_nonfatal_get_fade(level, i));
		put_text(level->ui.state.error_message[i], window, texture, rect);
		rect.y += UI_ELEMENT_HEIGHT;
		i++;
	}
}

t_ui_state	*get_ui_state(t_ui_state *get_state)
{
	static t_ui_state *state = NULL;

	if (get_state)
		state = get_state;
	else
		return (state);
	return (NULL);
}

static void	button_pixel_put(int x, int y, int color, unsigned *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return;
	texture[x + (y * RES_X)] = color;
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

t_ivec2			put_text(char *text, t_window *window, SDL_Texture *texture, t_ivec2 pos)
{
	static TTF_Font*	font = NULL;

	if (!font)
	{
		font = TTF_OpenFont("Roboto-Medium.ttf", UI_FONT_SIZE);
		if (!font)
		{
			printf("TTF_OpenFont: %s\n", TTF_GetError());
			ft_error("font open fail");
		}
	}
	// SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, get_text_color());
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, get_text_color());
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


static void	ui_render_background(unsigned *get_texture, SDL_Texture *text_texture, t_window *window, t_level *level)
{
	static unsigned	*texture;
	t_ui_state		*state;
	char			buf[100];

	if (get_texture)
	{
		texture = get_texture;
		return ;
	}
	state = get_ui_state(NULL);
	if (state->ssp_visual)
	{
		int color[3];
		color[0] = 0;
		color[1] = 0x66666644;
		color[2] = 0x66666688;
		for (int y = 0; y < RES_Y; y++)
			for (int x = 0; x < RES_X; x++)
			if (!texture[x + (y * RES_X)])
			{
				if (get_ssp_coordinate(x, 1) % 2 ^ get_ssp_coordinate(y, 0) % 2)
					button_pixel_put(x, y, color[1], texture);
				else
					button_pixel_put(x, y, color[2], texture);
			}
		int max_tris = 0;
		for (int y = 0; y < SSP_MAX_Y; y++)
			for (int x = 0; x < SSP_MAX_X; x++)
				if (max_tris < level->ssp[y * SSP_MAX_X + x].tri_amount)
					max_tris = level->ssp[y * SSP_MAX_X + x].tri_amount;
		for (int y = 0; y < SSP_MAX_Y; y++)
		{
			for (int x = 0; x < SSP_MAX_X; x++)
			{
				sprintf(buf, "%d", level->ssp[y * SSP_MAX_X + x].tri_amount);
				int red = (float)level->ssp[y * SSP_MAX_X + x].tri_amount / max_tris * 0xff;
				red = crossfade(0x00ff00, 0xff0000, red);
				set_text_color(red);
				put_text(buf, window, text_texture,
				(t_ivec2){(RES_X / SSP_MAX_X) * x + (RES_X / SSP_MAX_X / 2) - 5,
				(RES_Y / SSP_MAX_Y) * y + (RES_Y / SSP_MAX_Y / 2) - 7});
			}
		}
	}
	else
	{
		for (int y = 0; y < state->ui_text_y_pos + 6; y++)
			for (int x = 0; x < state->ui_max_width + UI_PADDING_4; x++)
				if (!texture[x + (y * RES_X)])
					button_pixel_put(x, y, UI_BACKGROUND_COL, texture);
	}
}

static t_ivec2	ui_render_internal(SDL_Texture *get_text, SDL_Texture *get_streaming, t_window *get_window, t_level *level, t_ui_state *state)
{
	static SDL_Texture *text_texture;
	static SDL_Texture *streaming_texture;
	static t_window *window;
	unsigned	*pixels = NULL;
	signed		width;
	t_ivec2 size;
	
	if (get_text)//on init
	{
		text_texture = get_text;
		streaming_texture = get_streaming;
		window = get_window;
		return (size);
	}
	if (state)//add line of text
	{
		t_ivec2 text_pos;
		text_pos.x = state->ui_text_x_offset;
		text_pos.y = state->ui_text_y_pos;
		size = put_text(state->text, window, text_texture, text_pos);
		if (state->ui_max_width < text_pos.x + size.x)
			state->ui_max_width = text_pos.x + size.x;
		state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
		return (size);
	}
	else//render
	{
		ui_render_nonfatal_errors(text_texture, window, level);
		ui_render_background(NULL, text_texture, window, level);
		SDL_UnlockTexture(streaming_texture);
		SDL_RenderCopy(window->SDLrenderer, streaming_texture, NULL, NULL);
		if (SDL_LockTexture(streaming_texture, NULL, (void**)&pixels, &width) != 0)
			ft_error("failed to lock texture\n");
		ft_memset(pixels, 0, RES_X * RES_Y * 4);
		SDL_RenderCopy(window->SDLrenderer, text_texture, NULL, NULL);
		SDL_SetRenderTarget(window->SDLrenderer, text_texture);
		SDL_RenderClear(window->SDLrenderer);
		SDL_RenderPresent(window->SDLrenderer);
		SDL_SetRenderTarget(window->SDLrenderer, NULL);
	}
	return (size);
}

static void	edit_slider_var(float *unit, t_ui_state *state)
{
	int			x;
	int			y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_drag &&
	y >= state->ui_text_y_pos + 4 &&
	y <= state->ui_text_y_pos + 15 && x < 109 && x > 2)
		*unit = (float)(x - 4) / (float)100;
}

static int	edit_call_var(t_ui_state *state, t_ivec2 size)
{
	int			x;
	int			y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_click &&
	x >= 3 && x <= size.x + 6 && y >= state->ui_text_y_pos + 4 && y <= state->ui_text_y_pos + size.y + 2)
		return (TRUE);
	return (FALSE);
}

static int	edit_button_var(int *var, t_ui_state *state)
{
	int			x;
	int			y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_click &&
	x >= 2 && x <= 11 && y >= state->ui_text_y_pos && y <= state->ui_text_y_pos + UI_ELEMENT_HEIGHT)
	{
		*var = *var ? FALSE : TRUE;
		return (TRUE);
	}
	return (FALSE);
}

static void	render_call_streaming(unsigned *get_texture, int dy, t_ivec2 *size, int color)
{
	static unsigned *texture;

	if (get_texture)
	{
		texture = get_texture;
		return ;
	}
	for (int y = 0; y < size->y - 1; y++)
	{
		for (int x = 0; x < size->x + 4; x++)
		{
			// if (y < 1 || y > 8 || x < 1 || x > 8)
				button_pixel_put(x + 2, y + 2 + dy, color, texture);
			// else
				// button_pixel_put(x + 2, y + 4 + dy, 0x303030ff, texture);
		}
	}
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
	ui_render_internal(NULL, NULL, NULL, NULL, state);
}

int		button(int *var, char *text)
{
	t_ui_state	*state;
	int			changed;

	state = get_ui_state(NULL);
	state->text = text;
	state->ui_text_x_offset = 14;
	render_button_streaming(NULL, var, state->ui_text_y_pos);
	changed = edit_button_var(var, state);
	ui_render_internal(NULL, NULL, NULL, NULL, state);
	return (changed);
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

	if (str)
		text(str);
	state = get_ui_state(NULL);
	state->text = "";
	state->ui_text_x_offset = 14;

	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider_streaming(NULL, unit, state->ui_text_y_pos);
	edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
}

void	float_slider(float *var, char *str, float min, float max)
{
	t_ui_state	*state;
	float		unit;

	if (str)
		text(str);
	state = get_ui_state(NULL);
	state->text = "";
	state->ui_text_x_offset = 14;

	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider_streaming(NULL, unit, state->ui_text_y_pos);
	edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
}

void	file_save(char *str, char *extension, void (*f)(t_level*, char*))
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	if (call(str, NULL, NULL))
	{
		state->is_serialize_open = TRUE;
		// state->title malloc extension;
		ft_strcpy(state->extension, extension);
		state->open_file = *f;;
	}
}

void	file_browser(char *str, char *extension, void (*f)(t_level*, char*))
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	if (call(str, NULL, NULL))
	{
		state->is_directory_open = TRUE;
		// state->title malloc extension;
		ft_strcpy(state->extension, extension);
		state->open_file = *f;;
	}
}

void	text_input(char *str, t_level *level)
{
	char *filename;

	if (str[0])
		filename = ft_strjoin(str, ".doom-nukem");
	if ((!str[0] && call("input:", NULL, NULL)) || (str[0] && call(filename, NULL, NULL)))
		level->ui.state.text_input_enable = TRUE;
	if (str[0])
		free(filename);
}

int		call(char *str, void (*f)(t_level*), t_level *level)
{
	int res = 0;
	t_ui_state	*state;
	int color_tmp;

	state = get_ui_state(NULL);
	state->text = str;
	state->ui_text_x_offset = 4;
	color_tmp = state->ui_text_color;
	state->ui_text_color = UI_BACKGROUND_COL;
	t_ivec2 size;
	size = ui_render_internal(NULL, NULL, NULL, NULL, state);
	state->ui_text_y_pos -= UI_ELEMENT_HEIGHT;
	state->ui_text_color = color_tmp;
	render_call_streaming(NULL, state->ui_text_y_pos, &size, state->ui_text_color);
	if (edit_call_var(state, size))
	{
		res = 1;
		if (*f)
			(*f)(level);
	}
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	return (res);
}

void	ui_render(t_level *level)
{
	level->ui.state.ui_max_width = 0;
	level->ui.state.ui_text_color = 0;
	level->ui.state.ui_text_x_offset = 0;
	level->ui.state.ui_text_y_pos = 0;
	get_ui_state(&level->ui.state);
	ui_config(level);
	ui_render_internal(NULL, NULL, NULL, level, NULL);
}

void	init_ui_state(t_level *level)
{
	if (!(level->ui.state.directory = (char*)malloc(sizeof(char) * PATH_MAX)))
		ft_error("memory allocation failed\n");
	if (!(level->ui.state.extension = (char*)malloc(sizeof(char) * NAME_MAX)))
		ft_error("memory allocation failed\n");
	if (!(level->ui.state.save_filename = (char*)malloc(sizeof(char) * NAME_MAX)))
		ft_error("memory allocation failed\n");

	ft_memset(level->ui.state.directory, 0, PATH_MAX - 1);
	ft_memset(level->ui.state.extension, 0, NAME_MAX - 1);
	ft_memset(level->ui.state.save_filename, 0, NAME_MAX - 1);
	level->ui.state.uv_pos.x = 0;
	level->ui.state.uv_pos.y = 0;
	level->ui.state.uv_zoom = 1;

#ifdef __APPLE__
	int path_max_size = PATH_MAX - 2;
	_NSGetExecutablePath(level->ui.state.directory, &path_max_size);
	path_up_dir(level->ui.state.directory);
#elif _WIN32
	TCHAR szFileName[PATH_MAX];
	GetModuleFileName(NULL, szFileName, PATH_MAX);
	ft_strcpy(level->ui.state.directory, szFileName);
#endif
	path_up_dir(level->ui.state.directory);
	go_in_dir(level->ui.state.directory, "level");
}

void	init_ui(t_window *window, t_level *level)
{
	//maybe this on windows
	SDL_Texture *text_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_X, RES_Y);
	// SDL_Texture *text_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	SDL_Texture *ui_texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	unsigned	*pixels;
	signed		width;
	t_editor_ui *ui;

	ui = &level->ui;
	ft_bzero(ui, sizeof(t_editor_ui));
	ui->noclip = TRUE;
	ui->blur = FALSE;
	ui->smooth_pixels = FALSE;
	ui->backface_culling = TRUE;
	ui->distance_culling = FALSE;
	ui->wireframe = FALSE;
	ui->wireframe_on_top = TRUE;
	ui->wireframe_culling_visual = TRUE;
	ui->render_distance = 20;
	ui->raycast_quality = NOISE_QUALITY_LIMIT - 1;
	// ui->fog_color = 0xffffffff;//fog
	// ui->fog_color = 0x000000ff;//night
	// ui->fog_color = 0xff0000ff;
	ui->fog_color = 0xb19a6aff;//sandstorm
	// ui->fog_color = 0xddddddff;//smoke

	ui->fov = M_PI / 2;
	ui->sun_contrast = 0;	//max 1
	ui->direct_shadow_contrast = 0;	//max 1
	ui->sun_dir.x = 1;
	ui->sun_dir.y = -1;
	ui->sun_dir.z = 1;
	vec_normalize(&ui->sun_dir);

	init_ui_state(level);
	TTF_Init();
	SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(ui_texture, SDL_BLENDMODE_BLEND);
	// if (SDL_LockTexture(text_texture, NULL, (void**)&pixels, &width) != 0)
	// 	ft_error("failed to lock texture\n");
	if (SDL_LockTexture(ui_texture, NULL, (void**)&pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	ui_render_internal(text_texture, ui_texture, window, NULL, NULL);
	render_button_streaming(pixels, 0, 0);
	render_slider_streaming(pixels, 0, 0);
	render_call_streaming(pixels, 0, NULL, 0);
	ui_render_background(pixels, NULL, NULL, NULL);
 }
