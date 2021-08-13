/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/13 23:25:47 by rpehkone         ###   ########.fr       */
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
	t_rthread	**thread_data;
	int			dummy_for_sdl;
	int			i;

	if (SDL_LockTexture(window->texture, NULL, (void **)&window->frame_buffer,
			&dummy_for_sdl) != 0)
		ft_error("failed to lock texture\n");
	if (!level->ui.wireframe || (level->ui.wireframe
			&& level->ui.wireframe_on_top))
	{
		i = 0;
		thread_data = (t_rthread **)malloc(sizeof(t_rthread *) * THREAD_AMOUNT);
		if (!thread_data)
			ft_error("memory allocation failed\n");
		while (i < THREAD_AMOUNT)
		{
			thread_data[i] = (t_rthread *)malloc(sizeof(t_rthread));
			if (!thread_data[i])
				ft_error("memory allocation failed\n");
			thread_data[i]->id = i;
			thread_data[i]->level = level;
			thread_data[i]->window = window;
			threads[i] = SDL_CreateThread(raycast, "asd",
					(void *)thread_data[i]);
			i++;
		}
		i = 0;
		while (i < THREAD_AMOUNT)
		{
			SDL_WaitThread(threads[i], &dummy_for_sdl);
			free(thread_data[i]);
			i++;
		}
		free(thread_data);
		fill_pixels(window->frame_buffer, level->ui.raycast_quality,
			level->ui.blur, level->ui.smooth_pixels);
	}
	SDL_UnlockTexture(window->texture);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
}

static void	render_raster(t_window *window, t_level *level)
{
	int	dummy_for_sdl;

	if (SDL_LockTexture(window->raster_texture, NULL, (void **)&window->raster_texture_pixels,
			&dummy_for_sdl) != 0)
		ft_error("failed to lock texture\n");
	wireframe(window->raster_texture_pixels, level);
	gizmo_render(level, window->raster_texture_pixels);
	SDL_UnlockTexture(window->raster_texture);
	SDL_RenderCopy(window->SDLrenderer, window->raster_texture, NULL, NULL);
	return ;
}

static void	render_ui(t_window *window, t_level *level, t_game_state *game_state)
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
	render_raycast(window, level);
	if (*game_state == GAME_STATE_EDITOR)
		render_raster(window, level);
	render_ui(window, level, game_state);
	SDL_RenderPresent(window->SDLrenderer);
}

int	main(int argc, char **argv)
{
	t_window		*window;
	t_level			*level;
	t_game_state	game_state;
	unsigned int	ssp_time;
	unsigned int	cull_time;
	unsigned int	render_time;
	unsigned int	frame_time;

	game_state = GAME_STATE_MAIN_MENU;
	game_state = GAME_STATE_EDITOR; // remove
	level = init_level();
	init_audio(level);
	init_window(&window);
	init_ui(window, level);
	init_screen_space_partition(level);
	init_culling(level);
	init_player(&level->player);
	open_level(level, "level/demo.doom-nukem");
	while (1)
	{
		frame_time = SDL_GetTicks();
		read_input(window, level, &game_state);
		if (game_state == GAME_STATE_MAIN_MENU)
			main_menu_move_background(level);
		else
		{
			if (game_state != GAME_STATE_EDITOR)
				game_logic(level, &game_state);
			if (game_state != GAME_STATE_DEAD)
				player_movement(level, game_state);
		}
		update_camera(level);
		door_animate(level);
		enemies_update_sprites(level);
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
