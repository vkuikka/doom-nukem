/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/06 08:50:56 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/21 03:39:14 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	button_pixel_put(int x, int y, int color, unsigned int *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return ;
	texture[x + (y * RES_X)] = color;
}

void	set_text_color(int color)
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	state->ui_text_color = color;
}

static SDL_Color	get_text_color(void)
{
	unsigned int	get;
	t_ui_state		*state;

	state = get_ui_state(NULL);
	return (get_sdl_color(state->ui_text_color));
}

static t_ivec2	render_text_internal(char *str, t_window *window,
						TTF_Font *font, t_ivec2 pos)
{
	SDL_Rect		text_rect;
	t_ivec2			size;
	SDL_Surface		*surfaceMessage;
	SDL_Texture		*Message;

	surfaceMessage
		= TTF_RenderText_Blended(font, str, get_text_color());
	Message
		= SDL_CreateTextureFromSurface(window->SDLrenderer, surfaceMessage);
	text_rect.w = 0;
	text_rect.h = 0;
	TTF_SizeText(font, str, &text_rect.w, &text_rect.h);
	text_rect.x = pos.x;
	text_rect.y = pos.y;
	SDL_SetRenderTarget(window->SDLrenderer, window->text_texture);
	SDL_RenderCopy(window->SDLrenderer, Message, NULL, &text_rect);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	size.x = text_rect.w;
	size.y = text_rect.h;
	return (size);
}

t_ivec2	render_text(char *str, int x, int y)
{
	t_ivec2		pos;
	t_window	*window;
	t_ui_state	*state;
	TTF_Font	*font;

	window = get_window(NULL);
	state = get_ui_state(NULL);
	font = state->current_font;
	pos.x = x;
	pos.y = y;
	return (render_text_internal(str, window, font, pos));
}

void	render_text_3d(char *str, t_vec3 pos,
			unsigned int color, t_level *level)
{
	set_text_color(color);
	camera_offset(&pos, &level->cam);
	if (pos.z > 0)
		render_text(str, pos.x, pos.y);
}

void	render_ssp_visual_background(unsigned int *texture)
{
	int	x;
	int	y;

	y = -1;
	while (++y < RES_Y)
	{
		x = -1;
		while (++x < RES_X)
		{
			if (!texture[x + (y * RES_X)])
			{
				if (get_ssp_coordinate(x, 1) % 2
					^ get_ssp_coordinate(y, 0) % 2)
					button_pixel_put(x, y, SSP_VISUAL_CHESSBOARD_1, texture);
				else
					button_pixel_put(x, y, SSP_VISUAL_CHESSBOARD_2, texture);
			}
		}
	}
}

static void	ssp_visual_text_text(t_level *level,
				int max_tris, int x, int y)
{
	char	buf[100];
	int		color;

	sprintf(buf, "%d", level->ssp[y * SSP_MAX_X + x].tri_amount);
	color = (float)level->ssp[y * SSP_MAX_X + x].tri_amount
		/ max_tris * 0xff;
	color = crossfade(0x00ff00, 0xff0000, color, 0xff);
	set_text_color(color);
	render_text(buf,
		(RES_X / SSP_MAX_X) * x + (RES_X / SSP_MAX_X / 2) - 5,
		(RES_Y / SSP_MAX_Y) * y + (RES_Y / SSP_MAX_Y / 2) - 7);
}

void	render_ssp_visual_text(t_level *level)
{
	int	max_tris;
	int	x;
	int	y;

	max_tris = 0;
	y = -1;
	while (++y < SSP_MAX_Y)
	{
		x = -1;
		while (++x < SSP_MAX_X)
			if (max_tris < level->ssp[y * SSP_MAX_X + x].tri_amount)
				max_tris = level->ssp[y * SSP_MAX_X + x].tri_amount;
	}
	y = -1;
	while (++y < SSP_MAX_Y)
	{
		x = -1;
		while (++x < SSP_MAX_X)
			ssp_visual_text_text(level, max_tris, x, y);
	}
}

void	ui_render_background(t_window *window, t_level *level)
{
	int	x;
	int	y;

	y = -1;
	while (++y < level->ui.state.ui_text_y_pos + 6)
	{
		x = -1;
		while (++x < level->ui.state.ui_max_width + UI_PADDING_4)
			if (!window->ui_texture_pixels[x + (y * RES_X)])
				button_pixel_put(x, y, UI_BACKGROUND_COL,
					window->ui_texture_pixels);
	}
}

#ifdef __APPLE__
void	init_ui_state(t_level *level)
{
	int		path_max_size;

	level->ui.state.directory = (char *)malloc(sizeof(char) * PATH_MAX);
	if (!level->ui.state.directory)
		ft_error("memory allocation failed\n");
	level->ui.state.extension = (char *)malloc(sizeof(char) * NAME_MAX);
	if (!level->ui.state.extension)
		ft_error("memory allocation failed\n");
	level->ui.state.save_filename = (char *)malloc(sizeof(char) * NAME_MAX);
	if (!level->ui.state.save_filename)
		ft_error("memory allocation failed\n");
	ft_memset(level->ui.state.directory, 0, PATH_MAX - 1);
	ft_memset(level->ui.state.extension, 0, NAME_MAX - 1);
	ft_memset(level->ui.state.save_filename, 0, NAME_MAX - 1);
	level->ui.state.uv_pos.x = 0;
	level->ui.state.uv_pos.y = 0;
	level->ui.state.uv_zoom = 1;
	path_max_size = PATH_MAX - 2;
	_NSGetExecutablePath(level->ui.state.directory, &path_max_size);
	path_up_dir(level->ui.state.directory);
	path_up_dir(level->ui.state.directory);
	go_in_dir(level->ui.state.directory, "level");
}

#elif _WIN32
void	init_ui_state(t_level *level)
{
	int		path_max_size;
	TCHAR	szFileName[PATH_MAX];

	level->ui.state.directory = (char *)malloc(sizeof(char) * PATH_MAX);
	if (!level->ui.state.directory)
		ft_error("memory allocation failed\n");
	level->ui.state.extension = (char *)malloc(sizeof(char) * NAME_MAX);
	if (!level->ui.state.extension)
		ft_error("memory allocation failed\n");
	level->ui.state.save_filename = (char *)malloc(sizeof(char) * NAME_MAX);
	if (!level->ui.state.save_filename)
		ft_error("memory allocation failed\n");
	ft_memset(level->ui.state.directory, 0, PATH_MAX - 1);
	ft_memset(level->ui.state.extension, 0, NAME_MAX - 1);
	ft_memset(level->ui.state.save_filename, 0, NAME_MAX - 1);
	level->ui.state.uv_pos.x = 0;
	level->ui.state.uv_pos.y = 0;
	level->ui.state.uv_zoom = 1;
	GetModuleFileName(NULL, szFileName, PATH_MAX);
	ft_strcpy(level->ui.state.directory, szFileName);
	path_up_dir(level->ui.state.directory);
	go_in_dir(level->ui.state.directory, "level");
}
#endif

static void	init_fonts(t_editor_ui *ui)
{
	TTF_Init();
	ui->editor_font = TTF_OpenFont("embed/Roboto-Medium.ttf", UI_FONT_SIZE);
	ui->hud_font = TTF_OpenFont("embed/digital.ttf", HUD_FONT_SIZE);
	ui->main_menu_font = TTF_OpenFont("embed/Roboto-Medium.ttf",
			MAIN_MENU_FONT_SIZE);
	ui->win_lose_font = TTF_OpenFont("embed/Roboto-Medium.ttf",
			HUD_GAME_EVENT_FONT_SIZE);
	if (!ui->editor_font || !ui->hud_font
		|| !ui->main_menu_font || !ui->win_lose_font)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		ft_error("font open fail");
	}
}

static void	init_ui_settings(t_level *level)
{
	t_editor_ui	*ui;

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
	ui->fog_color = 0xddddddff;
	ui->fov = M_PI / 2;
	ui->sun_color.r = 1;
	ui->sun_color.g = 1;
	ui->sun_color.b = 1;
	ui->sun_dir.x = 1;
	ui->sun_dir.y = -1;
	ui->sun_dir.z = 1;
	vec_normalize(&ui->sun_dir);
	ui->spray_from_view = 1;
	ui->spray_size = 3;
}

void	init_ui(t_window *window, t_level *level)
{
	int				width;

	window->text_texture = SDL_CreateTexture(window->SDLrenderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, RES_X, RES_Y);
	window->ui_texture = SDL_CreateTexture(window->SDLrenderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
			RES_X, RES_Y);
	init_ui_settings(level);
	init_ui_state(level);
	SDL_SetTextureBlendMode(window->text_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(window->ui_texture, SDL_BLENDMODE_BLEND);
	if (SDL_LockTexture(window->ui_texture, NULL,
			(void **)&window->ui_texture_pixels, &width) != 0)
		ft_error("failed to lock texture\n");
	init_fonts(&level->ui);
	get_ui_state(&level->ui.state);
	get_window(window);
}
