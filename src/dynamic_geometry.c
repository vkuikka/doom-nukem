/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dynamic_geometry.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 13:50:22 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	visible_request_merge(t_level *level, int amount)
{
	int	prev_amount;

	if (!level->visible_max)
		level->visible_max = 100;
	if (amount + level->visible.tri_amount
		>= level->visible_max)
	{
		prev_amount = level->visible_max;
		while (amount + level->visible.tri_amount
			>= level->visible_max)
			level->visible_max *= 1.5;
		level->visible.tris = (t_tri *)ft_realloc(level->visible.tris,
				sizeof(t_tri) * prev_amount,
				sizeof(t_tri) * level->visible_max);
		if (!level->visible.tris)
			ft_error("memory allocation failed");
	}
}

void	merge_mesh(t_level *level, t_obj *obj, t_vec3 pos, t_vec2 rotation)
{
	int	i;
	int	k;
	int	z;

	visible_request_merge(level, obj->tri_amount);
	k = -1;
	i = level->visible.tri_amount;
	while (++k < obj->tri_amount)
	{
		level->visible.tris[i] = obj->tris[k];
		level->visible.tris[i].index = 0;
		z = -1;
		while (++z < 3 + obj->tris[k].isquad)
		{
			rotate_vertex(rotation.x, &level->visible.tris[i].verts[z].pos, 1);
			rotate_vertex(rotation.y, &level->visible.tris[i].verts[z].pos, 0);
			vec_add(&level->visible.tris[i].verts[z].pos,
				level->visible.tris[i].verts[z].pos, pos);
		}
		tri_optimize(&level->visible.tris[i]);
		level->visible.tris[i].texture = &obj->texture;
		i++;
	}
	level->visible.tri_amount = i;
}

void	merge_enemies_runtime(t_level *level)
{
	int	i;

	i = -1;
	while (++i < level->game_logic.enemy_amount)
	{
		if (level->game_logic.enemies[i].dead_start_time)
			play_animation(&level->game_models.enemy,
				&level->game_models.enemy_die,
				level->game_logic.enemies[i].dead_start_time);
		else
		{
			if (level->game_logic.enemies[i].shoot_start_time)
				obj_copy(&level->game_models.enemy,
					&level->game_models.enemy_shoot);
			else
				play_animation(&level->game_models.enemy,
					&level->game_models.enemy_run, 0);
		}
		merge_mesh(level, &level->game_models.enemy,
			level->game_logic.enemies[i].pos,
			(t_vec2){0, level->game_logic.enemies[i].dir_rad});
	}
}

void	merge_enemies_editor(t_level *level, float rot)
{
	int	i;

	if (level->game_logic.enemy_animation_view_index == 0)
		play_animation(&level->game_models.enemy,
			&level->game_models.enemy_run, 0);
	else if (level->game_logic.enemy_animation_view_index == 1)
		play_animation(&level->game_models.enemy,
			&level->game_models.enemy_die, 0);
	else
		obj_copy(&level->game_models.enemy, &level->game_models.enemy_shoot);
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		merge_mesh(level, &level->game_models.enemy,
			level->game_logic.enemies[i].spawn_pos,
			(t_vec2){0, rot + (M_PI / 3 * i)});
}

void	merge_pickups(t_level *level, float rot)
{
	int	i;

	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
		if (level->game_logic.ammo_box[i].visible)
			merge_mesh(level, &level->game_models.ammo_pickup_box,
				level->game_logic.ammo_box[i].pos,
				(t_vec2){0, rot + (M_PI / 3 * i)});
	i = -1;
	while (++i < level->game_logic.health_box_amount)
		if (level->game_logic.health_box[i].visible)
			merge_mesh(level, &level->game_models.health_pickup_box,
				level->game_logic.health_box[i].pos,
				(t_vec2){0, rot + (M_PI / 3 * i)});
}

void	merge_game_models(t_level *level, t_game_state game_state)
{
	static float	rot = 0;
	int				i;

	rot += level->game_logic.item_spin_speed;
	if ((game_state == GAME_STATE_EDITOR
			&& level->ui.state.ui_location == UI_LOCATION_GAME_SETTINGS)
		|| game_state == GAME_STATE_INGAME)
	{
		merge_pickups(level, rot);
		if (game_state == GAME_STATE_INGAME)
			merge_enemies_runtime(level);
		else
			merge_enemies_editor(level, rot);
	}
	i = -1;
	while (++i < level->game_logic.projectile_amount)
		merge_sprite(level, level->game_logic.projectiles[i].pos,
			&level->game_models.projectile_sprite);
	if (game_state != GAME_STATE_EDITOR)
		return ;
	i = -1;
	while (++i < level->light_amount)
		merge_sprite(level, level->lights[i].pos,
			&level->game_models.light_sprite);
}

t_vec2	viewmodel_sway(t_level *level)
{
	static t_vec2	delta_vel = {0, 0};
	t_vec2			res;

	delta_vel.x -= level->cam.look_side;
	delta_vel.y -= level->cam.look_up;
	delta_vel.x *= .3;
	delta_vel.y *= .8;
	delta_vel.x = clamp(delta_vel.x, -0.2, 0.2);
	delta_vel.y = clamp(delta_vel.y, -0.6, 0.6);
	res = delta_vel;
	delta_vel.x = level->cam.look_side;
	delta_vel.y = level->cam.look_up;
	return (res);
}

void	merge_viewmodel(t_level *level)
{
	t_vec2	sway;

	sway = viewmodel_sway(level);
	merge_mesh(level, &level->game_models.viewmodel, level->cam.pos,
		(t_vec2){level->cam.look_up + sway.y, level->cam.look_side + sway.x});
}
