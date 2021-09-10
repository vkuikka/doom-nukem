/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/02 19:15:07 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	update_camera(t_level *l)
{
	l->cam.lon = -l->cam.look_side + M_PI / 2;
	l->cam.lat = -l->cam.look_up;
	rot_cam(&l->cam.front, l->cam.lon, l->cam.lat);
	rot_cam(&l->cam.up, l->cam.lon, l->cam.lat + (M_PI / 2));
	vec_cross(&l->cam.side, l->cam.up, l->cam.front);
	l->cam.fov_y = l->ui.fov;
	l->cam.fov_x = l->ui.fov * ((float)RES_X / RES_Y);
}

static void	render_raycast(t_window *window, t_level *level)
{
	SDL_Thread	*threads[THREAD_AMOUNT];
	t_rthread	thread_data[THREAD_AMOUNT];
	int			dummy_for_sdl;
	int			i;

	if (SDL_LockTexture(window->texture, NULL,
			(void **)&window->frame_buffer, &dummy_for_sdl) != 0)
		ft_error("failed to lock texture\n");
	i = -1;
	while (++i < THREAD_AMOUNT)
	{
		thread_data[i].id = i;
		thread_data[i].level = level;
		thread_data[i].window = window;
		threads[i] = SDL_CreateThread(init_raycast, "asd",
				(void *)&thread_data[i]);
	}
	i = -1;
	while (++i < THREAD_AMOUNT)
		SDL_WaitThread(threads[i], &dummy_for_sdl);
	fill_pixels(window->frame_buffer, level->ui.raycast_quality,
		level->ui.blur, level->ui.smooth_pixels);
	chromatic_abberation(window->frame_buffer, window->buf, level->ui.chromatic_abberation);
	SDL_UnlockTexture(window->texture);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
}

static void	render_raster(t_window *window, t_level *level,
							t_game_state *game_state)
{
	int	dummy_for_sdl;

	if (SDL_LockTexture(window->raster_texture, NULL,
			(void **)&window->raster_texture_pixels, &dummy_for_sdl
		) != 0)
		ft_error("failed to lock texture\n");
	ft_memset(window->raster_texture_pixels, 0, RES_X * RES_Y * sizeof(int));
	if (*game_state == GAME_STATE_EDITOR)
	{
		wireframe(window->raster_texture_pixels, level);
		gizmo_render(level, window->raster_texture_pixels);
	}
	SDL_UnlockTexture(window->raster_texture);
	SDL_RenderCopy(window->SDLrenderer, window->raster_texture, NULL, NULL);
	return ;
}

static void	render_ui(t_window *window, t_level *level,
						t_game_state *game_state)
{
	int	dummy_for_sdl;

	if (SDL_LockTexture(window->ui_texture, NULL,
			(void **)&window->ui_texture_pixels, &dummy_for_sdl) != 0)
		ft_error("failed to lock texture\n");
	ft_memset(window->ui_texture_pixels, 0, RES_X * RES_Y * 4);
	ui(window, level, game_state);
	SDL_UnlockTexture(window->ui_texture);
	SDL_RenderCopy(window->SDLrenderer, window->ui_texture, NULL, NULL);
	SDL_RenderCopy(window->SDLrenderer, window->text_texture, NULL, NULL);
	SDL_SetRenderTarget(window->SDLrenderer, window->text_texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderPresent(window->SDLrenderer);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
}

static void	render(t_window *window, t_level *level, t_game_state *game_state)
{
	SDL_RenderClear(window->SDLrenderer);
	if (level->level_initialized)
	{
		if (!level->ui.wireframe
			|| (level->ui.wireframe && level->ui.wireframe_on_top))
			render_raycast(window, level);
		render_raster(window, level, game_state);
	}
	render_ui(window, level, game_state);
	SDL_RenderPresent(window->SDLrenderer);
}

static void	tick_forward(t_level *level, t_game_state *game_state)
{
	if (*game_state == GAME_STATE_MAIN_MENU)
		main_menu_move_background(level);
	else
	{
		if (*game_state != GAME_STATE_EDITOR)
			game_logic(level, game_state);
		if (*game_state != GAME_STATE_DEAD)
			player_movement(level);
	}
	update_camera(level);
	door_animate(level);
	enemies_update_sprites(level);
}

static void	dnukem(t_window *window, t_level *level, t_game_state game_state)
{
	unsigned int	ssp_time;
	unsigned int	cull_time;
	unsigned int	render_time;
	unsigned int	frame_time;

	while (1)
	{
		frame_time = SDL_GetTicks();
		read_input(window, level, &game_state);
		tick_forward(level, &game_state);
		cull_time = SDL_GetTicks();
		culling(level);
		level->ui.cull_time = SDL_GetTicks() - cull_time;
		ssp_time = SDL_GetTicks();
		screen_space_partition(level);
		level->ui.ssp_time = SDL_GetTicks() - ssp_time;
		render_time = SDL_GetTicks();
		render(window, level, &game_state);
		level->ui.render_time = SDL_GetTicks() - render_time;
		level->ui.frame_time = SDL_GetTicks() - frame_time;
	}
}

int	main(int argc, char **argv)
{
	t_window		*window;
	t_level			*level;
	t_game_state	game_state;

	(void)argc;
	(void)argv;
	game_state = GAME_STATE_MAIN_MENU;
	level = (t_level *)malloc(sizeof(t_level));
	if (!level)
		ft_error("memory allocation failed\n");
	ft_bzero(level, sizeof(t_level));
	init_embedded(level);
	init_audio(level);
	init_window(&window);
	init_ui(window, level);
	init_player(&level->player);

	//remove
	open_level(level, "level/demo.doom-nukem");
	game_state = GAME_STATE_EDITOR;
	level->cam.pos = level->spawn_pos.pos;
	level->cam.look_side = level->spawn_pos.look_side;
	level->cam.look_up = level->spawn_pos.look_up;
	//remove

	while (!level->level_initialized)
	{
		read_input(window, level, &game_state);
		render(window, level, &game_state);
	}
	dnukem(window, level, game_state);
	return (0);
}
