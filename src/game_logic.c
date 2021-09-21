/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/10 22:47:18 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/14 18:50:26 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	player_reload(t_level *level)
{
	float	time;

	time = (SDL_GetTicks() - level->game_logic.reload_start_time)
		/ (1000 * VIEWMODEL_ANIM_FPS);
	if (time < 1)
		level->viewmodel_index = (int)(time * VIEWMODEL_FRAMES);
	else
	{
		level->viewmodel_index = 0;
		level->game_logic.reload_start_time = 0;
		level->game_logic.player_ammo = PLAYER_AMMO_MAX;
	}
}

static void	player_shoot(t_level *level)
{
	if (level->game_logic.player_ammo)
	{
		level->game_logic.player_ammo--;
		level->player.dir = level->cam.front;
		create_projectile(level, level->cam.pos,
			level->cam.front, &level->player);
	}
	else
		level->game_logic.reload_start_time = SDL_GetTicks();
}

static void	game_finished(t_level *level, t_game_state *game_state,
				float time)
{
	if (level->game_logic.win_start_time)
	{
		time = (SDL_GetTicks() - level->game_logic.win_start_time)
			/ (1000.0 * WIN_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->game_logic.win_start_time = 0;
		}
	}
	else if (level->game_logic.death_start_time)
	{
		level->game_logic.player_health = 0;
		time = (SDL_GetTicks() - level->game_logic.death_start_time)
			/ (1000.0 * DEATH_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->game_logic.death_start_time = 0;
		}
	}
}

static float	obj_find_lowest_point(t_obj *obj)
{
	float	min;
	int		i;
	int		k;

	min = 0;
	i = 0;
	while (i < obj->tri_amount)
	{
		k = 0;
		while (k < 3 + obj->tris[i].isquad)
		{
			if (obj->tris[i].verts[k].pos.y > min)
				min = obj->tris[i].verts[k].pos.y;
			k++;
		}
		i++;
	}
	return (min);
}

static void	obj_pos_set_to_floor(t_vec3 *vec, t_obj *obj, t_level *level)
{
	t_ray	ray;
	float	ground_dist;

	ray.dir = (t_vec3){0, 1, 0};
	ray.pos = *vec;
	ground_dist = cast_all(ray, level, NULL);
	if (ground_dist == FLT_MAX)
		ground_dist = 0;
	else
		ground_dist -= obj_find_lowest_point(obj);
	vec->y += ground_dist;
}

void	add_health_box(t_level *level)
{
	int	amount;

	amount = level->game_logic.health_box_amount;
	level->game_logic.health_box_spawn_pos = (t_vec3 *)ft_realloc(level->game_logic.health_box_spawn_pos,
		sizeof(t_vec3) * amount, sizeof(t_vec3) * (amount + 1));
	if (!level->game_logic.health_box_spawn_pos)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.health_box_spawn_pos[amount],
		level->cam.pos, level->cam.front);
	obj_pos_set_to_floor(&level->game_logic.health_box_spawn_pos[amount],
			&level->game_models.pickup_box, level);
	level->game_logic.health_box_amount++;
}

void	add_ammo_box(t_level *level)
{
	int	amount;

	amount = level->game_logic.ammo_box_amount;
	level->game_logic.ammo_box_spawn_pos = (t_vec3 *)ft_realloc(level->game_logic.ammo_box_spawn_pos,
		sizeof(t_vec3) * amount, sizeof(t_vec3) * (amount + 1));
	if (!level->game_logic.ammo_box_spawn_pos)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.ammo_box_spawn_pos[amount],
		level->cam.pos, level->cam.front);
	obj_pos_set_to_floor(&level->game_logic.ammo_box_spawn_pos[amount],
			&level->game_models.pickup_box, level);
	level->game_logic.ammo_box_amount++;
}

void	add_enemy_spawn_pos(t_level *level)
{
	int	amount;

	amount = level->game_logic.enemy_amount;
	level->game_logic.enemy_spawn_pos = (t_vec3 *)ft_realloc(level->game_logic.enemy_spawn_pos,
		sizeof(t_vec3) * amount, sizeof(t_vec3) * (amount + 1));
	if (!level->game_logic.enemy_spawn_pos)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.enemy_spawn_pos[amount],
		level->cam.pos, level->cam.front);
	obj_pos_set_to_floor(&level->game_logic.enemy_spawn_pos[amount],
			&level->game_models.pickup_box, level);
	level->game_logic.enemy_amount++;
}

void	game_logic_move_selected(t_level *level, t_vec3 move_amount)
{
	(void)level;
	(void)move_amount;
}

void	game_logic_select_nearest_to_mouse(t_level *level, int x, int y)
{
	(void)level;
	(void)x;
	(void)y;
}

void	game_logic_put_info(t_level *level, unsigned int *texture)
{
	t_vec3	dist;
	int		i;

	draw_camera_path("menu", &level->main_menu_anim, texture, level);
	render_text_3d("spawn", level->game_logic.spawn_pos.pos,
		UI_LEVEL_SETTINGS_TEXT_COLOR, level);
	vec_sub(&dist, level->game_logic.win_pos, level->cam.pos);
	if (vec_length(dist) < level->game_logic.win_dist)
		render_text_3d("win", level->game_logic.win_pos, UI_LEVEL_BAKED_COLOR, level);
	else
		render_text_3d("win", level->game_logic.win_pos, UI_LEVEL_NOT_BAKED_COLOR, level);
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
		render_text_3d("ammo", level->game_logic.ammo_box_spawn_pos[i], AMMO_BOX_TEXT_COLOR, level);
	i = -1;
	while (++i < level->game_logic.health_box_amount)
		render_text_3d("health", level->game_logic.health_box_spawn_pos[i], HEALTH_BOX_TEXT_COLOR, level);
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		render_text_3d("enemy", level->game_logic.enemy_spawn_pos[i], ENEMY_SPAWN_TEXT_COLOR, level);
}

void	game_logic(t_level *level, t_game_state *game_state)
{
	t_vec3	dist;

	handle_audio(level, game_state);
	if (level->game_logic.reload_start_time && *game_state != GAME_STATE_DEAD)
		player_reload(level);
	if (level->ui.state.m1_click && level->ui.state.mouse_capture
		&& *game_state != GAME_STATE_DEAD)
		player_shoot(level);
	if (level->game_logic.win_start_time || level->game_logic.death_start_time)
		game_finished(level, game_state, 0);
	else if (level->game_logic.player_health <= 0)
	{
		level->game_logic.reload_start_time = 0;
		level->game_logic.death_start_time = SDL_GetTicks();
		*game_state = GAME_STATE_DEAD;
		level->cam.look_up = 1;
	}
	else if (*game_state == GAME_STATE_INGAME)
	{
		vec_sub(&dist, level->game_logic.win_pos, level->cam.pos);
		if (vec_length(dist) < level->game_logic.win_dist)
			*game_state = GAME_STATE_WIN;
		if (*game_state == GAME_STATE_WIN)
			level->game_logic.win_start_time = SDL_GetTicks();
	}
}
