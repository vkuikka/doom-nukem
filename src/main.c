/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 13:52:31 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	update_camera(t_level *l)
{
	l->cam.lon = -l->cam.look_side + M_PI / 2;
	l->cam.lat = -l->cam.look_up;
	rot_cam(&l->cam.front, l->cam.lon, l->cam.lat);
	rot_cam(&l->cam.up, l->cam.lon, l->cam.lat + (M_PI / 2));
	vec_cross(&l->cam.side, l->cam.up, l->cam.front);
	l->cam.fov_y = tanf(l->ui.fov / 2);
	l->cam.fov_x = tanf(l->ui.fov / 2) * ((float)RES_X / RES_Y);
}

void	render_raster(t_window *window, t_level *level)
{
	if (SDL_LockTexture(window->raster_texture, NULL,
			(void **)&window->raster_texture_pixels, &(int){0}) != 0)
		ft_error("failed to lock texture\n");
	memset(window->raster_texture_pixels, 0, RES_X * RES_Y * sizeof(int));
	wireframe(window->raster_texture_pixels, level);
	if (level->ui.state.gizmo_active)
		gizmo_render(level, window->raster_texture_pixels);
	SDL_UnlockTexture(window->raster_texture);
	SDL_RenderCopy(window->SDLrenderer, window->raster_texture, NULL, NULL);
}

void	render_ui(t_window *window, t_level *level,
						t_game_state *game_state)
{
	int	dummy_for_sdl;

	if (SDL_LockTexture(window->ui_texture, NULL,
			(void **)&window->ui_texture_pixels, &dummy_for_sdl) != 0)
		ft_error("failed to lock texture\n");
	memset(window->ui_texture_pixels, 0, RES_X * RES_Y * 4);
	ui(window, level, game_state);
	SDL_UnlockTexture(window->ui_texture);
	SDL_RenderCopy(window->SDLrenderer, window->ui_texture, NULL, NULL);
	SDL_RenderCopy(window->SDLrenderer, window->text_texture, NULL, NULL);
	SDL_SetRenderTarget(window->SDLrenderer, window->text_texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderPresent(window->SDLrenderer);
	SDL_SetRenderTarget(window->SDLrenderer, NULL);
}

void	raycast_finish(t_window *window, t_level *level)
{
	level->ui.post_time = SDL_GetTicks();
	post_process(window, level);
	level->ui.post_time = SDL_GetTicks() - level->ui.post_time;
	SDL_UnlockTexture(window->texture);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
}

void	render(t_window *window, t_level *level, t_game_state *game_state)
{
	unsigned int	raster_time;
	unsigned int	ui_time;

	if (level->level_initialized)
	{
		level->ui.total_raycasts = 0;
		level->ui.raycast_time = SDL_GetTicks();
		if (!level->ui.wireframe
			|| (level->ui.wireframe && level->ui.wireframe_on_top))
		{
			render_raycast(window, level, game_state);
			raycast_finish(window, level);
		}
		level->ui.raycast_time = SDL_GetTicks()
			- level->ui.raycast_time - level->ui.post_time;
		raster_time = SDL_GetTicks();
		if (*game_state == GAME_STATE_EDITOR)
			render_raster(window, level);
		level->ui.raster_time = SDL_GetTicks() - raster_time;
	}
	ui_time = SDL_GetTicks();
	render_ui(window, level, game_state);
	level->ui.ui_time = SDL_GetTicks() - ui_time;
	SDL_RenderPresent(window->SDLrenderer);
	SDL_RenderClear(window->SDLrenderer);
}

void	tick_forward(t_level *level, t_game_state *game_state)
{
	if (*game_state == GAME_STATE_MAIN_MENU)
		camera_path_set(&level->main_menu_anim, &level->cam);
	else
	{
		if (*game_state != GAME_STATE_EDITOR)
			game_logic(level, game_state);
		player_movement(level);
	}
	update_camera(level);
	door_animate(level);
	if (*game_state == GAME_STATE_INGAME)
	{
		projectiles_update(level);
		enemies_update(level);
	}
}

void	viewmodel(t_window *window, t_level *level,
								t_game_state *game_state)
{
	int	normal_map_disabled;

	if (*game_state != GAME_STATE_INGAME)
		return ;
	if (level->game_logic.reload_start_time)
		play_animation(&level->game_models.viewmodel,
			&level->game_models.reload_animation,
			level->game_logic.reload_start_time);
	level->visible.tri_amount = 0;
	merge_viewmodel(level);
	screen_space_partition(level);
	level->render_is_first_pass = TRUE;
	normal_map_disabled = level->ui.normal_map_disabled;
	level->ui.normal_map_disabled = TRUE;
	render_raycast(window, level, game_state);
	level->ui.normal_map_disabled = normal_map_disabled;
	level->render_is_first_pass = FALSE;
}

void	dnukem(t_window *window, t_level *level, t_game_state game_state)
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
		viewmodel(window, level, &game_state);
		culling(level);
		merge_game_models(level, game_state);
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
	init_window(&window);
	init_ui(window, level);
	init_audio(level);
	init_embedded(level);
	Mix_PlayMusic(level->audio.title_music, -1);
	init_screen_space_partition(level);
	init_culling(level);
	while (!level->level_initialized)
	{
		read_input(window, level, &game_state);
		render(window, level, &game_state);
	}
	dnukem(window, level, game_state);
	return (0);
}
