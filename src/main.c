/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/10 22:41:35 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

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

static void	render(t_window *window, t_level *level, t_game_state *game_state)
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
	if (*game_state == GAME_STATE_EDITOR
		&& level->ui.state.ui_location != UI_LOCATION_SETTINGS)
		wireframe(window, level);
	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	if (*game_state == GAME_STATE_INGAME || *game_state == GAME_STATE_DEAD
		|| *game_state == GAME_STATE_WIN)
		hud(level, window, *game_state);
	else if (*game_state == GAME_STATE_MAIN_MENU)
		main_menu(level, window, game_state);
	else
	{
		gizmo_render(level, window);
		if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
			uv_editor(level, window);
		ui_render(level);
	}
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

static void	player_reload(t_level *level)
{
	float	time;

	time = (SDL_GetTicks() - level->reload_start_time)
		/ (1000 * VIEWMODEL_ANIM_FPS);
	if (time < 1)
		level->viewmodel_index = (int)(time * VIEWMODEL_FRAMES);
	else
	{
		level->viewmodel_index = 0;
		level->reload_start_time = 0;
		level->player_ammo = PLAYER_AMMO_MAX;
	}
}

void	game_logic(t_level *level, t_game_state *game_state)
{
	t_vec3	dist;
	float	time;

	handle_audio(level, game_state);
	if (level->reload_start_time && *game_state != GAME_STATE_DEAD)
		player_reload(level);
	if (level->ui.state.m1_click && level->ui.state.mouse_capture
		&& *game_state != GAME_STATE_DEAD)
	{
		if (level->player_ammo)
		{
			level->player_ammo--;
			level->player.dir = level->cam.front;
			create_projectile(
				level, level->cam.pos, level->cam.front, &level->player);
		}
		else
		{
			level->reload_start_time = SDL_GetTicks();
		}
	}
	if (level->win_start_time)
	{
		time = (SDL_GetTicks() - level->win_start_time)
			/ (1000.0 * WIN_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->win_start_time = 0;
		}
	}
	else if (level->death_start_time)
	{
		level->player_health = 0;
		time = (SDL_GetTicks() - level->death_start_time)
			/ (1000.0 * DEATH_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->death_start_time = 0;
		}
	}
	else if (level->player_health <= 0)
	{
		level->reload_start_time = 0;
		level->death_start_time = SDL_GetTicks();
		*game_state = GAME_STATE_DEAD;
		level->cam.look_up = 1;
	}
	else if (*game_state == GAME_STATE_INGAME)
	{
		vec_sub(&dist, level->win_pos, level->cam.pos);
		if (vec_length(dist) < level->win_dist)
		{
			*game_state = GAME_STATE_WIN;
			level->win_start_time = SDL_GetTicks();
		}
	}
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
	start_bake(level);
	while (1)
	{
		frame_time = SDL_GetTicks();
		read_input(window, level, &game_state);
		if (game_state == GAME_STATE_MAIN_MENU)
			main_menu_move_background(level);
		else
		{
			if (game_state == GAME_STATE_INGAME
				|| game_state == GAME_STATE_DEAD
				|| game_state == GAME_STATE_WIN)
				game_logic(level, &game_state);
			if (game_state != GAME_STATE_DEAD)
				player_movement(level, game_state);
		}
		update_camera(level);
		door_animate(level);
		if (game_state == GAME_STATE_EDITOR)
		{
			door_put_text(window, level);
			light_put_text(window, level);
		}
		enemies_update_sprites(level);
		cull_time = SDL_GetTicks();
		culling(level);
		level->ui.cull = SDL_GetTicks() - cull_time;
		ssp_time = SDL_GetTicks();
		screen_space_partition(level);
		level->ui.ssp = SDL_GetTicks() - ssp_time;
		render_time = SDL_GetTicks();
		render(window, level, &game_state);
		level->ui.render = SDL_GetTicks() - render_time;
		level->ui.frametime = SDL_GetTicks() - frame_time;
	}
}
