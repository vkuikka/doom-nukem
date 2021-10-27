/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projectile.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 09:19:15 by rpehkone          #+#    #+#             */
/*   Updated: 2021/10/27 23:03:08 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	create_projectile(t_game_logic *logic, t_vec3 pos,
								t_vec3 dir, t_projectile get)
{
	int		prev_amount;
	int		i;

	if (logic->projectile_amount + 1 >= logic->projectile_max)
	{
		prev_amount = logic->projectile_max;
		if (!logic->projectile_max)
			logic->projectile_max = 10;
		else
			logic->projectile_max *= 1.5;
		logic->projectiles = (t_projectile *)ft_realloc(logic->projectiles,
				sizeof(t_projectile) * prev_amount,
				sizeof(t_projectile) * logic->projectile_max);
		if (!logic->projectiles)
			ft_error("memory allocation failed");
	}
	i = logic->projectile_amount;
	logic->projectiles[i] = get;
	logic->projectiles[i].pos = pos;
	logic->projectiles[i].dir = dir;
	vec_mult(&dir, 3);
	vec_add(&logic->projectiles[i].pos,
		logic->projectiles[i].pos, dir);
	logic->projectile_amount++;
}

static void	remove_projectile(t_game_logic *logic, int i)
{
	while (i < logic->projectile_amount - 1)
	{
		logic->projectiles[i] = logic->projectiles[i + 1];
		i++;
	}
	logic->projectile_amount--;
}

static int	projectile_collision_dynamic(t_projectile *projectile,
								t_level *level)
{
	int		i;
	t_vec3	tmp;

	vec_sub(&tmp, level->cam.pos, projectile->pos);
	if (vec_length(tmp) <= PROJECTILE_DAMAGE_DIST)
	{
		vec_mult(&level->game_logic.player.vel, 0.5);
		level->game_logic.player.health -= projectile->damage;
		return (TRUE);
	}
	i = 0;
	while (i < level->game_logic.enemy_amount)
	{
		vec_sub(&tmp, level->game_logic.enemies[i].pos, projectile->pos);
		if (vec_length(tmp) <= PROJECTILE_DAMAGE_DIST)
		{
			level->game_logic.enemies[i].remaining_health -= projectile->damage;
			return (TRUE);
		}
		i++;
	}
	return (FALSE);
}

static int	projectile_collision_static(t_projectile *projectile,
								t_level *level, t_vec3 next_pos)
{
	int		hit_index;
	float	dist;
	t_ray	e;

	e.dir = projectile->dir;
	e.pos = projectile->pos;
	dist = cast_all(e, level, &hit_index);
	if (dist < vec_dist(projectile->pos, next_pos))
	{
		if (level->all.tris[hit_index].isbreakable)
		{
			level->all.tris[hit_index].isbroken = 1;
			return (FALSE);
		}
		return (TRUE);
	}
	return (FALSE);
}

void	projectiles_update(t_level *level)
{
	t_projectile	*projectile;
	t_vec3			move_amount;
	t_vec3			next_pos;
	float			time;
	int				i;

	i = 0;
	while (i < level->game_logic.projectile_amount)
	{
		projectile = &level->game_logic.projectiles[i];
		time = level->ui.frame_time / 1000.0;
		move_amount = projectile->dir;
		vec_mult(&move_amount, projectile->speed * time);
		vec_add(&next_pos, projectile->pos, move_amount);
		if (projectile_collision_static(projectile, level, next_pos)
			|| projectile_collision_dynamic(projectile, level))
			remove_projectile(&level->game_logic, i);
		else
			projectile->pos = next_pos;
		i++;
	}
}
