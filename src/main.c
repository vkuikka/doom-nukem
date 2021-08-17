/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/18 01:03:26 by vkuikka          ###   ########.fr       */
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
		ft_memset(window->frame_buffer, 0, RES_X * RES_Y * 4);
		while (i < THREAD_AMOUNT)
		{
			thread_data[i] = (t_rthread *)malloc(sizeof(t_rthread));
			if (!thread_data[i])
				ft_error("memory allocation failed\n");
			thread_data[i]->id = i;
			thread_data[i]->level = level;
			thread_data[i]->window = window;
			// threads[i] = SDL_CreateThread(raycast, "asd", (void*)thread_data[i]);
			threads[i] = SDL_CreateThread(raster, "asd", (void*)thread_data[i]);
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
		// fill_pixels(window->frame_buffer, level->ui.raycast_quality,
		// 			level->ui.blur, level->ui.smooth_pixels);
	}
	SDL_UnlockTexture(window->texture);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
}

static void	render_raster(t_window *window, t_level *level, t_game_state *game_state)
{
	int	dummy_for_sdl;

	if (SDL_LockTexture(window->raster_texture, NULL, (void **)&window->raster_texture_pixels,
			&dummy_for_sdl) != 0)
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
	render_raster(window, level, game_state);
	render_ui(window, level, game_state);
	SDL_RenderPresent(window->SDLrenderer);
}

void	update_screen_space_vertices(t_level *level)
{
	t_vec3 start;
	t_vec3 stop;

	int	z = 0;
	level->ss_tri_amount = 0;
	for (int i = 0; i < level->visible.tri_amount; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			int		next;
			next = (k + 1) % 3;
			start = level->visible.tris[i].verts[k].pos;
			stop = level->visible.tris[i].verts[next].pos;
			camera_offset(&start, &level->cam);
			camera_offset(&stop, &level->cam);
			if (start.z < 0)
				start = move2z(&stop, &start);
			level->ss_tris[z].verts[k].pos = start;
			level->ss_tris[z].verts[k].uv = level->visible.tris[i].verts[k].txtr;
		}
		z++;
		if (level->visible.tris[i].isquad)
		{
			for (int k = 0; k < 3; k++)
			{
				int		next;
				int		next2;
				next = (int[3]){1, 3, 2}[k];
				next2 = (int[3]){1, 3, 2}[(k + 1) % 3];
				start = level->visible.tris[i].verts[next].pos;
				stop = level->visible.tris[i].verts[next2].pos;
				camera_offset(&start, &level->cam);
				camera_offset(&stop, &level->cam);
				if (start.z < 0)
					start = move2z(&start, &stop);
				level->ss_tris[z].verts[k].pos = stop;
				level->ss_tris[z].verts[k].uv = level->visible.tris[i].verts[next].txtr;
			}
			z++;
		}
	}
	level->ss_tri_amount = z;
}

void	insertion_sort_tris(t_level *level)
{
	int		i;
	t_obj	*obj = &level->all;
	t_tri	key;
	int		j;
	t_vec3	sub;

	i = 0;
	while (i < obj->tri_amount)
	{
		obj->tris[i].dist = FLT_MIN;
		j = 0;
		while (j < 3)
		{
			vec_sub(&sub, obj->tris[i].verts[j].pos, level->cam.pos);
			if (vec_length(sub) > obj->tris[i].dist)
				obj->tris[i].dist = vec_length(sub);
			j++;
		}
		i++;
	}

	for (i = 1; i < obj->tri_amount; i++)
	{
		key = obj->tris[i];
		j = i - 1;
		while (j >= 0 && obj->tris[j].dist > key.dist)
		{
			obj->tris[j + 1] = obj->tris[j];
			j--;
		}
		obj->tris[j + 1] = key;
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
	// init_screen_space_partition(level);
	level->ui.backface_culling = FALSE;
	level->ui.raycast_quality = 1;
	init_culling(level);
	init_player(&level->player);
	open_level(level, "level/demo.doom-nukem");
	level->ss_tris = malloc(sizeof(t_ss_tri) * 30000);
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
		insertion_sort_tris(level);
		enemies_update_sprites(level);
		cull_time = SDL_GetTicks();
		culling(level);
		update_screen_space_vertices(level);
		level->ui.cull_time = SDL_GetTicks() - cull_time;
		ssp_time = SDL_GetTicks();
		// screen_space_partition(level);
		level->ui.ssp_time = SDL_GetTicks() - ssp_time;
		render_time = SDL_GetTicks();
		render(window, level, &game_state);
		level->ui.render_time = SDL_GetTicks() - render_time;
		level->ui.frame_time = SDL_GetTicks() - frame_time;
	}
}
