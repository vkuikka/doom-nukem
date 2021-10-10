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

static void	move_enemy(t_enemy *enemy, t_level *level, float time,
					t_enemy_settings *settings)
{
	t_ray	e;
	float	dist;
	t_vec3	player;
	t_vec3	tmp;

	enemy->pos.y -= 1.5;
	player = level->cam.pos;
	e.pos.x = 0;
	e.pos.y = 0;
	e.pos.z = 0;
	e.pos = enemy->pos;
	vec_sub(&e.dir, player, e.pos);
	dist = cast_all(e, level, NULL);
	if (player.y > e.pos.y - ENEMY_MOVABLE_HEIGHT_DIFF
		&& player.y < e.pos.y + ENEMY_MOVABLE_HEIGHT_DIFF)
	{
		if (dist > vec_length(e.dir))
			enemy->dir = e.dir;
		if ((dist > vec_length(e.dir)
				&& vec_length(e.dir) > settings->dist_limit)
			|| dist < vec_length(e.dir) - settings->dist_limit)
		{
			if (enemy->dir.x || enemy->dir.z)
			{
				e.dir = enemy->dir;
				e.dir.y = 0;
				vec_normalize(&e.dir);
				vec_mult(&e.dir, settings->move_speed * time);
				tmp = e.dir;
				vec_add(&e.pos, e.pos, e.dir);
				vec_sub(&tmp, enemy->dir, e.dir);
				if (vec_dot(tmp, e.dir) > 0)
				{
					enemy->dir = tmp;
					vec_add(&enemy->pos, enemy->pos, e.dir);
				}
			}
		}
	}
	obj_pos_set_to_floor(&enemy->pos, &level->game_models.enemy, level);
	enemy->dir_rad = -1 * atan2(enemy->dir.z, enemy->dir.x) - M_PI / 2;
}

static void	enemy_attack(t_enemy *enemy, t_level *level, float time)
{
	t_projectile		*projectile_settings;
	t_enemy_settings	*settings;
	t_vec3				tmp;

	projectile_settings = &level->game_logic.enemy_projectile_settings;
	settings = &level->game_logic.enemy_settings;
	vec_sub(&tmp, level->cam.pos, enemy->pos);
	enemy->current_attack_delay += time;
	//if calc dist to enemy (enemy see dist) tai laita anemy->can_see_player
	if (enemy->current_attack_delay >= settings->attack_frequency)
	{
		if (vec_length(tmp) < settings->melee_range)
		{
			enemy->current_attack_delay = 0;
			vec_mult(&level->game_logic.player.vel, 0.5);
			level->game_logic.player.health -= projectile_settings->damage;
		}
		else if (projectile_settings->speed)
		{
			enemy->current_attack_delay = 0;
			t_vec3 pos = enemy->pos;
			pos.y -= 1.0;
			t_vec3	dir;
			vec_sub(&dir, level->cam.pos, pos);
			vec_normalize(&dir);
			create_projectile(&level->game_logic, pos, dir,
				level->game_logic.enemy_projectile_settings);
		}
	}
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

static void	enemy_spawn(t_enemy *enemy, t_enemy_settings *settings)
{
	enemy->dead_start_time = 0;
	enemy->current_attack_delay = 0;
	enemy->dir = (t_vec3){1, 0, 0};
	enemy->dir_rad = 0;
	enemy->pos = enemy->spawn_pos;
	enemy->remaining_health = settings->initial_health;
}

void	spawn_enemies(t_level *level)
{
	int	i;

	i = 0;
	while (i < level->game_logic.enemy_amount)
	{
		enemy_spawn(&level->game_logic.enemies[i],
			&level->game_logic.enemy_settings);
		i++;
	}
}

void	enemies_update(t_level *level)
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
	i = 0;
	while (i < level->game_logic.enemy_amount)
	{
		if (!level->game_logic.enemies[i].dead_start_time)
		{
			if (level->game_logic.enemies[i].remaining_health <= 0)
				level->game_logic.enemies[i].dead_start_time = SDL_GetTicks();
			else
			{
				move_enemy(&level->game_logic.enemies[i], level, time,
					&level->game_logic.enemy_settings);
				enemy_attack(&level->game_logic.enemies[i], level, time);
			}
		}
		i++;
	}
}
