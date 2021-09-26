/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/25 16:54:52 by rpehkone         ###   ########.fr       */
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

static void	render_raycast(t_window *window, t_level *level, t_game_state *game_state)
{
	SDL_Thread	*threads[THREAD_AMOUNT];
	t_rthread	thread_data[THREAD_AMOUNT];
	int			i;

	if (SDL_LockTexture(window->texture, NULL,
			(void **)&window->frame_buffer, &(int){0}) != 0)
		ft_error("failed to lock texture\n");
	if (level->render_is_first_pass || *game_state != GAME_STATE_INGAME)
		ft_memset(window->frame_buffer, 0, RES_X * RES_Y * sizeof(int));
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
		SDL_WaitThread(threads[i], &(int){0});
}

static void	render_raster(t_window *window, t_level *level)
{
	if (SDL_LockTexture(window->raster_texture, NULL,
			(void **)&window->raster_texture_pixels, &(int){0}) != 0)
		ft_error("failed to lock texture\n");
	ft_memset(window->raster_texture_pixels, 0, RES_X * RES_Y * sizeof(int));
	wireframe(window->raster_texture_pixels, level);
	if (level->ui.state.gizmo_active)
		gizmo_render(level, window->raster_texture_pixels);
	SDL_UnlockTexture(window->raster_texture);
	SDL_RenderCopy(window->SDLrenderer, window->raster_texture, NULL, NULL);
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
	unsigned int	raycast_time;
	unsigned int	raster_time;
	unsigned int	ui_time;

	if (level->level_initialized)
	{
		level->ui.total_raycasts = 0;
		raycast_time = SDL_GetTicks();
		if (!level->ui.wireframe
			|| (level->ui.wireframe && level->ui.wireframe_on_top))
		{
			render_raycast(window, level, game_state);
			level->ui.post_time = SDL_GetTicks();
			post_process(window, level);
			level->ui.post_time = SDL_GetTicks() - level->ui.post_time;
			SDL_UnlockTexture(window->texture);
			SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
		}
		level->ui.raycast_time = SDL_GetTicks() - raycast_time - level->ui.post_time;
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

static void	tick_forward(t_level *level, t_game_state *game_state)
{
	if (*game_state == GAME_STATE_MAIN_MENU)
		camera_path_set(&level->main_menu_anim, &level->cam);
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

static void	merge_prop(t_level *level, t_obj *obj, t_vec3 pos, t_vec2 rotation)
{
	int	i;
	int	k;

	if (!level->visible_max)
		level->visible_max = level->all.tri_amount;
	if (obj->tri_amount + level->visible.tri_amount
		>= level->visible_max)
	{
		level->visible.tris = (t_tri *)ft_realloc(level->visible.tris,
				sizeof(t_tri) * level->visible_max,
				sizeof(t_tri) * (int)(level->visible_max * 1.5));
		level->visible_max *= 1.5;
		if (!level->visible.tris)
			ft_error("memory allocation failed");
	}
	k = 0;
	i = level->visible.tri_amount;
	while (k < obj->tri_amount)
	{
		level->visible.tris[i] = obj->tris[k];
		for (int z = 0; z < 3 + obj->tris[k].isquad; z++)
		{
			if (rotation.x)
				rotate_vertex(rotation.x, &level->visible.tris[i].verts[z].pos, 1);
			if (rotation.y)
				rotate_vertex(rotation.y, &level->visible.tris[i].verts[z].pos, 0);
			level->visible.tris[i].verts[z].pos.x += pos.x;
			level->visible.tris[i].verts[z].pos.y += pos.y;
			level->visible.tris[i].verts[z].pos.z += pos.z;
		}
		if (rotation.y)
		{
			vec_sub(&level->visible.tris[i].v0v2, level->visible.tris[i].verts[1].pos,
				level->visible.tris[i].verts[0].pos);
			vec_sub(&level->visible.tris[i].v0v1, level->visible.tris[i].verts[2].pos,
				level->visible.tris[i].verts[0].pos);
			vec_cross(&level->visible.tris[i].normal, level->visible.tris[i].v0v2,
				level->visible.tris[i].v0v1);
			vec_normalize(&level->visible.tris[i].normal);
		}
		level->visible.tris[i].texture = &obj->texture;
		i++;
		k++;
	}
	level->visible.tri_amount = i;
}

static void	merge_game_models(t_level *level, t_game_state game_state)
{
	static float	rot = 0;
	int				i;

	rot += .03;
	if ((game_state == GAME_STATE_EDITOR
		&& level->ui.state.ui_location == UI_LOCATION_GAME_SETTINGS)
		|| game_state == GAME_STATE_INGAME)
	{
		i = -1;
		while (++i < level->game_logic.ammo_box_amount)
			if (level->game_logic.ammo_box[i].visible)
				merge_prop(level, &level->game_models.ammo_pickup_box,
					level->game_logic.ammo_box[i].pos, (t_vec2){0, rot + (M_PI / 3 * i)});
		i = -1;
		while (++i < level->game_logic.health_box_amount)
			if (level->game_logic.health_box[i].visible)
				merge_prop(level, &level->game_models.health_pickup_box,
					level->game_logic.health_box[i].pos, (t_vec2){0, rot + (M_PI / 3 * i)});
		i = -1;
		while (++i < level->game_logic.enemy_amount)
			merge_prop(level, &level->game_models.enemy,
				level->game_logic.enemy_spawn_pos[i], (t_vec2){0, rot + (M_PI / 3 * i)});
	}
}

static void	viewmodel(t_window *window, t_level *level, t_game_state *game_state)
{
	if (*game_state != GAME_STATE_INGAME)
		return ;
	level->visible.tri_amount = 0;
	merge_prop(level, &level->game_models.viewmodel, level->cam.pos, (t_vec2){level->cam.look_up, level->cam.look_side});
	screen_space_partition(level);
	level->render_is_first_pass = TRUE;
	render_raycast(window, level, game_state);
	level->render_is_first_pass = FALSE;
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
	init_player(&level->player);
	Mix_PlayMusic(level->audio.title_music, -1);
	while (!level->level_initialized)
	{
		read_input(window, level, &game_state);
		render(window, level, &game_state);
	}
	dnukem(window, level, game_state);
	return (0);
}
