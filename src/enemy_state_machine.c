/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemy_state_machine.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 09:35:17 by rpehkone          #+#    #+#             */
/*   Updated: 2021/11/08 17:16:58 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	enemy_turn(t_enemy *enemy)
{
	vec_sub(&enemy->dir, enemy->move_to, enemy->pos);
	enemy->dir.y = 0;
	vec_normalize(&enemy->dir);
	enemy->dir_rad = -1 * atan2(enemy->dir.z, enemy->dir.x) - M_PI / 2;
}

static void	enemy_vision(t_enemy *enemy, t_level *level,
					t_enemy_settings *settings)
{
	float	enemy_view_dist;
	t_vec3	player;
	float	dist;
	t_ray	e;

	enemy->can_see_player = FALSE;
	player = level->cam.pos;
	e.pos = enemy->pos;
	e.pos.y -= 1.7;
	vec_sub(&e.dir, player, e.pos);
	dist = cast_all(e, level, NULL);
	if (player.y > e.pos.y - ENEMY_MOVABLE_HEIGHT_DIFF
		&& player.y < e.pos.y + ENEMY_MOVABLE_HEIGHT_DIFF)
	{
		if (level->ui.fog)
			enemy_view_dist = level->ui.render_distance;
		else
			enemy_view_dist = settings->dist_limit;
		if (dist > vec_length(e.dir) && dist < enemy_view_dist)
		{
			enemy->can_see_player = TRUE;
			enemy->move_to = level->cam.pos;
			enemy_turn(enemy);
		}
	}
}

static void	enemy_attack(t_enemy *enemy, t_level *level)
{
	t_enemy_settings	*settings;
	t_vec3				tmp;
	t_vec3				pos;

	settings = &level->game_logic.enemy_settings;
	if (SDL_GetTicks() < enemy->current_attack_delay
		+ settings->attack_frequency)
		return ;
	enemy->current_attack_delay = SDL_GetTicks();
	vec_sub(&tmp, level->cam.pos, enemy->pos);
	if (vec_length(tmp) < settings->melee_range)
	{
		vec_mult(&level->game_logic.player.vel, 0.5);
		level->game_logic.player.health -= settings->melee_damage;
	}
	else if (level->game_logic.enemy_projectile_settings.speed)
	{
		pos = enemy->pos;
		pos.y -= 1.0;
		create_projectile(&level->game_logic, pos, enemy->dir,
			level->game_logic.enemy_projectile_settings);
	}
}

static void	enemy_move(t_enemy *enemy, t_level *level)
{
	float	time;
	t_vec3	tmp;

	time = (SDL_GetTicks() - enemy->move_start_time)
		/ (1000.0 * level->game_logic.enemy_settings.move_duration);
	if (time >= 1.0)
	{
		enemy->move_start_time = 0;
		enemy->shoot_start_time = SDL_GetTicks();
	}
	tmp = enemy->dir;
	vec_mult(&tmp, level->game_logic.enemy_settings.move_speed
		* (level->ui.frame_time / 1000.0));
	vec_add(&enemy->pos, enemy->pos, tmp);
	enemy->pos.y -= 1.5;
	if (!obj_pos_set_to_floor(&enemy->pos, &level->game_models.enemy, level))
		enemy->pos.y += 1.5;
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
		enemy_move(enemy, level);
	else
	{
		time = (SDL_GetTicks() - enemy->shoot_start_time)
			/ (1000.0 * level->game_logic.enemy_settings.shoot_duration);
		if (time >= 1.0)
		{
			enemy->shoot_start_time = 0;
			enemy->move_start_time = SDL_GetTicks();
		}
		else if (enemy->can_see_player)
			enemy_attack(enemy, level);
	}
}
