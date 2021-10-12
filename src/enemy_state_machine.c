/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_state_machine.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 09:35:17 by rpehkone          #+#    #+#             */
/*   Updated: 2021/10/12 11:33:10 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	enemy_look_at_player(t_enemy *enemy, t_level *level)
{
	vec_sub(&enemy->dir, level->cam.pos, enemy->pos);
	enemy->dir.y = 0;
	vec_normalize(&enemy->dir);
	enemy->dir_rad = -1 * atan2(enemy->dir.z, enemy->dir.x) - M_PI / 2;
}

static void	enemy_vision(t_enemy *enemy, t_level *level,
					t_enemy_settings *settings)
{
	t_ray	e;
	float	dist;
	t_vec3	player;

	enemy->can_see_player = FALSE;
	player = level->cam.pos;
	e.pos.x = 0;
	e.pos.y = 0;
	e.pos.z = 0;
	e.pos = enemy->pos;
	e.pos.y -= 1.7;//enemy eye height
	vec_sub(&e.dir, player, enemy->pos);
	dist = cast_all(e, level, NULL);
	if (player.y > e.pos.y - ENEMY_MOVABLE_HEIGHT_DIFF
		&& player.y < e.pos.y + ENEMY_MOVABLE_HEIGHT_DIFF)
	{
		if ((dist > vec_length(e.dir)
				&& vec_length(e.dir) > settings->dist_limit)
			|| dist < vec_length(e.dir) - settings->dist_limit)
		{
			enemy->can_see_player = TRUE;
			enemy_look_at_player(enemy, level);
		}
	}
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

void	enemy_state_machine(t_enemy *enemy, t_level *level)
{
	float	time;

	if (enemy->remaining_health <= 0)
	{
		enemy->dead_start_time = SDL_GetTicks();
		return ;
	}
	enemy_vision(enemy, level, &level->game_logic.enemy_settings);
	if (enemy->move_start_time)
	{
		time = (SDL_GetTicks() - enemy->move_start_time)
			/ (1000.0 * level->game_logic.enemy_settings.move_duration);
		if (time >= 1.0)
		{
			enemy->move_start_time = 0;
			enemy->shoot_start_time = SDL_GetTicks();
		}
		if (enemy->can_see_player)
		{
			t_vec3 tmp = enemy->dir;
			vec_mult(&tmp, level->game_logic.enemy_settings.move_speed * (level->ui.frame_time / 1000.0));
			vec_add(&enemy->pos, enemy->pos, tmp);
			enemy->pos.y -= 1.5;//enemy max step
			obj_pos_set_to_floor(&enemy->pos, &level->game_models.enemy, level);
		}
	}
	else
	{
		time = (SDL_GetTicks() - enemy->shoot_start_time)
			/ (1000.0 * level->game_logic.enemy_settings.shoot_duration);
		if (time >= 1.0)
		{
			enemy->shoot_start_time = 0;
			enemy->move_start_time = SDL_GetTicks();
		}
		if (enemy->can_see_player)
			enemy_attack(enemy, level, time);
	}
}
