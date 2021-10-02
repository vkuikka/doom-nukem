/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 17:08:49 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/22 22:25:53 by vkuikka          ###   ########.fr       */
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

/*
static void	move_enemy(t_tri *face, t_level *level, float time)
{
	t_ray	e;
	float	dist;
	t_vec3	player;
	t_vec3	tmp;
	int		i;

	player = level->cam.pos;
	e.pos.x = 0;
	e.pos.y = 0;
	e.pos.z = 0;
	i = -1;
	while (++i < 3 + face->isquad)
		vec_add(&e.pos, e.pos, face->verts[i].pos);
	vec_div(&e.pos, 3 + face->isquad);
	vec_sub(&e.dir, player, e.pos);
	dist = cast_all(e, level, NULL);
	if (player.y > e.pos.y - ENEMY_MOVABLE_HEIGHT_DIFF
		&& player.y < e.pos.y + ENEMY_MOVABLE_HEIGHT_DIFF)
	{
		if (dist > vec_length(e.dir))
			face->enemy->dir = e.dir;
		if ((dist > vec_length(e.dir)
				&& vec_length(e.dir) > face->enemy->dist_limit)
			|| dist < vec_length(e.dir) - face->enemy->dist_limit)
		{
			if (face->enemy->dir.x || face->enemy->dir.z)
			{
				e.dir = face->enemy->dir;
				e.dir.y = 0;
				vec_normalize(&e.dir);
				vec_mult(&e.dir, face->enemy->move_speed * time);
				tmp = e.dir;
				vec_add(&e.pos, e.pos, e.dir);
				vec_sub(&tmp, face->enemy->dir, e.dir);
				if (vec_dot(tmp, e.dir) > 0)
				{
					face->enemy->dir = tmp;
					i = -1;
					while (++i < 3 + face->isquad)
						vec_add(&face->verts[i].pos, face->verts[i].pos, e.dir);
				}
			}
		}
	}
	vec_sub(&tmp, player, e.pos);
	face->enemy->current_attack_delay += time;
	if (dist > vec_length(tmp)
		&& face->enemy->current_attack_delay >= face->enemy->attack_frequency)
	{
		if (vec_length(tmp) < face->enemy->attack_range)
		{
			face->enemy->current_attack_delay = 0;
			vec_mult(&level->player_vel, 0);
			level->game_logic.player_health -= face->enemy->attack_damage;
		}
		else if (face->enemy->projectile_speed)
		{
			face->enemy->current_attack_delay = 0;
			vec_sub(&e.dir, player, e.pos);
			vec_normalize(&e.dir);
			create_projectile(level, e.pos, e.dir, face->enemy);
		}
	}
}
*/

static int	projectile_collision(t_projectile *projectile, t_level *level, float time)
{
	int		hit_index;
	float	dist;
	t_ray	e;

	e.pos = projectile->pos;
	vec_sub(&e.dir, level->cam.pos, e.pos);
	if (vec_length(e.dir) <= PROJECTILE_DAMAGE_DIST && projectile->damage > 0)
	{
		vec_mult(&level->game_logic.player.vel, 0.5);
		level->game_logic.player.health -= projectile->damage;
		return (TRUE);
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

void	enemies_update_sprites(t_level *level)
{
	t_vec3			move_amount;
	t_projectile	*proj;
	float			time;
	int				i;

	i = 0;
	time = level->ui.frame_time / 1000.0;
	while (i < level->game_logic.projectile_amount)
	{
		proj = &level->game_logic.projectiles[i];
		if (projectile_collision(proj, level, time))
			remove_projectile(&level->game_logic, i);
		else
		{
			move_amount = proj->dir;
			vec_mult(&move_amount, proj->speed * time);
			vec_add(&proj->pos, proj->pos, move_amount);
		}
		i++;
	}
}
