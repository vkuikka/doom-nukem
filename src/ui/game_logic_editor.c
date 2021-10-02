/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic_editor.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/25 15:29:26 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/25 15:55:49 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

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
	level->game_logic.health_box
		= (t_item_pickup *)ft_realloc(level->game_logic.health_box,
			sizeof(t_item_pickup) * amount,
			sizeof(t_item_pickup) * (amount + 1));
	if (!level->game_logic.health_box)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.health_box[amount].pos,
		level->cam.pos, level->cam.front);
	level->game_logic.health_box[amount].visible = 1;
	level->game_logic.health_box[amount].start_time = 0;
	obj_pos_set_to_floor(&level->game_logic.health_box[amount].pos,
		&level->game_models.health_pickup_box, level);
	level->game_logic.health_box_amount++;
}

void	add_ammo_box(t_level *level)
{
	int	amount;

	amount = level->game_logic.ammo_box_amount;
	level->game_logic.ammo_box
		= (t_item_pickup *)ft_realloc(level->game_logic.ammo_box,
			sizeof(t_item_pickup) * amount,
			sizeof(t_item_pickup) * (amount + 1));
	if (!level->game_logic.ammo_box)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.ammo_box[amount].pos,
		level->cam.pos, level->cam.front);
	level->game_logic.ammo_box[amount].visible = 1;
	level->game_logic.ammo_box[amount].start_time = 0;
	obj_pos_set_to_floor(&level->game_logic.ammo_box[amount].pos,
		&level->game_models.ammo_pickup_box, level);
	level->game_logic.ammo_box_amount++;
}

void	delete_health_box(t_level *level)
{
	int	amount;
	int	i;

	amount = level->game_logic.health_box_amount;
	i = level->ui.state.logic_selected_index;
	while (i < amount - 1)
	{
		level->game_logic.health_box[i] = level->game_logic.health_box[i + 1];
		i++;
	}
	level->game_logic.health_box
		= (t_item_pickup *)ft_realloc(level->game_logic.health_box,
			sizeof(t_item_pickup) * amount,
			sizeof(t_item_pickup) * (amount - 1));
	if (!level->game_logic.health_box)
		ft_error("memory allocation failed");
	level->game_logic.health_box_amount--;
	level->ui.state.logic_selected = GAME_LOGIC_SELECTED_NONE;
}

void	delete_ammo_box(t_level *level)
{
	int	amount;
	int	i;

	amount = level->game_logic.ammo_box_amount;
	i = level->ui.state.logic_selected_index;
	while (i < amount - 1)
	{
		level->game_logic.ammo_box[i] = level->game_logic.ammo_box[i + 1];
		i++;
	}
	level->game_logic.ammo_box
		= (t_item_pickup *)ft_realloc(level->game_logic.ammo_box,
			sizeof(t_item_pickup) * amount,
			sizeof(t_item_pickup) * (amount - 1));
	if (!level->game_logic.ammo_box)
		ft_error("memory allocation failed");
	level->game_logic.ammo_box_amount--;
	level->ui.state.logic_selected = GAME_LOGIC_SELECTED_NONE;
}

void	delete_enemy_spawn_pos(t_level *level)
{
	int	amount;
	int	i;

	amount = level->game_logic.enemy_amount;
	i = level->ui.state.logic_selected_index;
	while (i < amount - 1)
	{
		level->game_logic.enemies[i].spawn_pos
			= level->game_logic.enemies[i + 1].spawn_pos;
		i++;
	}
	level->game_logic.enemies
		= (t_enemy *)ft_realloc(level->game_logic.enemies,
			sizeof(t_enemy) * amount, sizeof(t_enemy) * (amount - 1));
	if (!level->game_logic.enemies)
		ft_error("memory allocation failed");
	level->game_logic.enemy_amount--;
	level->ui.state.logic_selected = GAME_LOGIC_SELECTED_NONE;
}

void	add_enemy_spawn_pos(t_level *level)
{
	int	amount;

	amount = level->game_logic.enemy_amount;
	level->game_logic.enemies
		= (t_enemy *)ft_realloc(level->game_logic.enemies,
			sizeof(t_enemy) * amount, sizeof(t_enemy) * (amount + 1));
	if (!level->game_logic.enemies)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.enemies[amount].spawn_pos,
		level->cam.pos, level->cam.front);
	obj_pos_set_to_floor(&level->game_logic.enemies[amount].spawn_pos,
		&level->game_models.enemy, level);
	level->game_logic.enemy_amount++;
}

static void	move_selected_vec(t_level *level, t_vec3 move_amount, t_vec3 *vec)
{
	vec_add(vec, *vec, move_amount);
	level->ui.state.gizmo_pos = *vec;
}

void	game_logic_move_selected(t_level *level, t_vec3 move)
{
	int	i;

	i = level->ui.state.logic_selected_index;
	level->ui.state.gizmo_active = TRUE;
	if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_NONE)
		level->ui.state.gizmo_active = FALSE;
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_SPAWN)
		move_selected_vec(level, move, &level->game_logic.spawn_pos.pos);
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_WIN)
		move_selected_vec(level, move, &level->game_logic.win_pos);
	else if (level->ui.state.logic_selected
		== GAME_LOGIC_SELECTED_MENU_ANIMATION)
		move_selected_vec(level, move, &level->main_menu_anim.pos[i].pos);
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_AMMO)
		move_selected_vec(level, move, &level->game_logic.ammo_box[i].pos);
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_HEALTH)
		move_selected_vec(level, move, &level->game_logic.health_box[i].pos);
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_ENEMY)
		move_selected_vec(level, move, &level->game_logic.enemies[i].spawn_pos);
}

static int	check_if_3d_closer(t_vec3 vert, float *dist, t_level *level)
{
	t_vec2	test;
	float	len;

	camera_offset(&vert, &level->cam);
	if (vert.z < 0 || vert.x < 0 || vert.x > RES_X || vert.y < 0
		|| vert.y > RES_Y)
		return (FALSE);
	test.x = vert.x;
	test.y = vert.y;
	test.x -= level->ui.state.mouse.x;
	test.y -= level->ui.state.mouse.y;
	len = vec2_length(test);
	if (len < *dist)
	{
		*dist = len;
		return (TRUE);
	}
	return (FALSE);
}

void	game_logic_select_nearest_to_mouse_item(t_level *level, float *dist)
{
	int	i;

	i = -1;
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
	{
		if (check_if_3d_closer(level->game_logic.ammo_box[i].pos, dist, level))
		{
			level->ui.state.logic_selected = GAME_LOGIC_SELECTED_AMMO;
			level->ui.state.logic_selected_index = i;
		}
	}
	i = -1;
	while (++i < level->game_logic.health_box_amount)
	{
		if (check_if_3d_closer(level->game_logic.health_box[i].pos, dist,
				level))
		{
			level->ui.state.logic_selected = GAME_LOGIC_SELECTED_HEALTH;
			level->ui.state.logic_selected_index = i;
		}
	}
}

void	game_logic_select_nearest_to_mouse_not_item(t_level *level, float *dist)
{
	int	i;

	i = -1;
	while (++i < level->main_menu_anim.amount)
	{
		if (check_if_3d_closer(level->main_menu_anim.pos[i].pos, dist, level))
		{
			level->ui.state.logic_selected = GAME_LOGIC_SELECTED_MENU_ANIMATION;
			level->ui.state.logic_selected_index = i;
		}
	}
	i = -1;
	while (++i < level->game_logic.enemy_amount)
	{
		if (check_if_3d_closer(level->game_logic.enemies[i].spawn_pos, dist,
				level))
		{
			level->ui.state.logic_selected = GAME_LOGIC_SELECTED_ENEMY;
			level->ui.state.logic_selected_index = i;
		}
	}
}

void	game_logic_select_nearest_to_mouse(t_level *level)
{
	float	dist;

	dist = FLT_MAX;
	level->ui.state.logic_selected = GAME_LOGIC_SELECTED_NONE;
	level->ui.state.logic_selected_index = 0;
	if (check_if_3d_closer(level->game_logic.spawn_pos.pos, &dist, level))
		level->ui.state.logic_selected = GAME_LOGIC_SELECTED_SPAWN;
	if (check_if_3d_closer(level->game_logic.win_pos, &dist, level))
		level->ui.state.logic_selected = GAME_LOGIC_SELECTED_WIN;
	game_logic_select_nearest_to_mouse_item(level, &dist);
	game_logic_select_nearest_to_mouse_not_item(level, &dist);
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
		render_text_3d("win", level->game_logic.win_pos,
			UI_LEVEL_BAKED_COLOR, level);
	else
		render_text_3d("win", level->game_logic.win_pos,
			UI_LEVEL_NOT_BAKED_COLOR, level);
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
		render_text_3d("ammo", level->game_logic.ammo_box[i].pos,
			AMMO_BOX_TEXT_COLOR, level);
	i = -1;
	while (++i < level->game_logic.health_box_amount)
		render_text_3d("health", level->game_logic.health_box[i].pos,
			HEALTH_BOX_TEXT_COLOR, level);
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		render_text_3d("enemy", level->game_logic.enemies[i].spawn_pos,
			ENEMY_SPAWN_TEXT_COLOR, level);
}
