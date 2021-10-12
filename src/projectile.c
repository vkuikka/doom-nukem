/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   projectile.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 09:19:15 by rpehkone          #+#    #+#             */
/*   Updated: 2021/10/12 11:34:27 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	create_projectile(t_game_logic *logic, t_vec3 pos,
								t_vec3 dir, t_projectile get)
{
	int prev_amount;
	int	i;

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
	logic->projectiles[i].pos = pos;
	t_vec3 tmp = dir;
	vec_mult(&tmp, 3);
	vec_add(&logic->projectiles[i].pos,
		logic->projectiles[i].pos, tmp);
	logic->projectiles[i].dir = dir;
	logic->projectiles[i].damage = get.damage;
	logic->projectiles[i].speed = get.speed;
	logic->projectiles[i].dist = get.dist;
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

static int	projectile_collision(t_projectile *projectile, t_level *level, float time)
{
	int		hit_index;
	float	dist;
	t_ray	e;
	int		i;

	e.pos = projectile->pos;
	vec_sub(&e.dir, level->cam.pos, e.pos);
	if (vec_length(e.dir) <= PROJECTILE_DAMAGE_DIST)
	{
		vec_mult(&level->game_logic.player.vel, 0.5);
		level->game_logic.player.health -= projectile->damage;
		return (TRUE);
	}
	i = 0;
	while (i < level->game_logic.enemy_amount)
	{
		e.pos = projectile->pos;
		vec_sub(&e.dir, level->game_logic.enemies[i].pos, e.pos);
		if (vec_length(e.dir) <= PROJECTILE_DAMAGE_DIST)
		{
			level->game_logic.enemies[i].remaining_health -= projectile->damage;
			return (TRUE);
		}
		i++;
	}
	e.dir = projectile->dir;
	dist = cast_all(e, level, &hit_index);
	vec_mult(&e.dir, projectile->speed * time);
	if (dist <= vec_length(e.dir)
		|| projectile->dist > MAX_PROJECTILE_TRAVEL)
	{
		// if (dist <= vec_length(e.dir) && hit_index > 0 && level->all.tris[hit_index].isbreakable)
			return (TRUE);
		// else
		// 	remove_projectile(level, index);
		// return;
	}
	return (FALSE);
}

void	projectiles_update(t_level *level)
{
	t_projectile	*projectile;
	t_vec3			move_amount;
	float			time;
	int				i;

	time = level->ui.frame_time / 1000.0;
	i = 0;
	while (i < level->game_logic.projectile_amount)
	{
		projectile = &level->game_logic.projectiles[i];
		if (projectile_collision(projectile, level, time))
			remove_projectile(&level->game_logic, i);
		else
		{
			move_amount = projectile->dir;
			vec_mult(&move_amount, projectile->speed * time);
			vec_add(&projectile->pos, projectile->pos, move_amount);
		}
		i++;
	}
}
