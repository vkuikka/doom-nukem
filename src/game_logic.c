/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 15:03:50 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	player_reload(t_level *level)
{
	float	time;

	time = (SDL_GetTicks() - level->game_logic.reload_start_time)
		/ (1000 * level->game_models.reload_animation.duration);
	if (time > 1)
	{
		level->game_logic.reload_start_time = 0;
		level->game_logic.player.ammo = PLAYER_AMMO_MAX;
	}
}

void	player_shoot(t_level *level)
{
	float	wall_dist;
	t_vec3	dir;

	if (level->game_logic.player.ammo)
	{
		if (!level->game_logic.reload_start_time)
		{
			dir = level->cam.front;
			vec_mult(&dir, PLAYER_PROJECTILE_START_POS);
			wall_dist = cast_all((t_ray){level->cam.pos, dir}, level, NULL);
			if (wall_dist > PLAYER_PROJECTILE_START_POS)
			{
				level->game_logic.player.ammo--;
				create_projectile(&level->game_logic, level->cam.pos,
					level->cam.front,
					level->game_logic.player_projectile_settings);
			}
		}
	}
	else
		level->game_logic.reload_start_time = SDL_GetTicks();
}

void	game_finished(t_level *level, t_game_state *game_state)
{
	float	time;

	time = 0;
	if (level->game_logic.win_start_time)
	{
		time = (SDL_GetTicks() - level->game_logic.win_start_time)
			/ (1000.0 * WIN_LENGTH_SEC);
	}
	else if (level->game_logic.death_start_time)
	{
		level->game_logic.player.health = 0;
		time = (SDL_GetTicks() - level->game_logic.death_start_time)
			/ (1000.0 * DEATH_LENGTH_SEC);
	}
	if (time > 1)
		ui_go_back(level, game_state);
}

int	pick_up_pick_ups(t_level *level, t_item_pickup *pickups, int amount)
{
	t_vec3	dist;
	int		i;

	i = -1;
	while (++i < amount)
	{
		if (pickups[i].start_time)
		{
			if (SDL_GetTicks() - pickups[i].start_time
				> ITEM_SPAWN_TIME * 1000.0)
			{
				pickups[i].start_time = 0;
				pickups[i].visible = TRUE;
			}
			continue ;
		}
		vec_sub(&dist, pickups[i].pos, level->cam.pos);
		if (vec_length(dist) < ITEM_PICKUP_DIST)
		{
			pickups[i].start_time = SDL_GetTicks();
			pickups[i].visible = FALSE;
			return (TRUE);
		}
	}
	return (FALSE);
}

void	reset_pick_ups(t_level *level)
{
	int	i;

	i = -1;
	while (++i < level->game_logic.health_box_amount)
	{
		level->game_logic.health_box[i].start_time = 0;
		level->game_logic.health_box[i].visible = TRUE;
	}
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
	{
		level->game_logic.ammo_box[i].start_time = 0;
		level->game_logic.ammo_box[i].visible = TRUE;
	}
}

void	game_logic_win_lose(t_level *level, t_game_state *game_state)
{
	t_vec3	dist;

	if (level->game_logic.player.health <= 0)
	{
		level->game_logic.reload_start_time = 0;
		level->game_logic.death_start_time = SDL_GetTicks();
		level->cam.look_up = 1;
	}
	else if (*game_state == GAME_STATE_INGAME)
	{
		vec_sub(&dist, level->game_logic.win_pos, level->cam.pos);
		if (vec_length(dist) < level->game_logic.win_dist)
			level->game_logic.win_start_time = SDL_GetTicks();
	}
}

void	game_logic(t_level *level, t_game_state *game_state)
{
	handle_audio(level);
	if (level->game_logic.win_start_time || level->game_logic.death_start_time)
		game_finished(level, game_state);
	if (level->game_logic.death_start_time)
		return ;
	if (level->game_logic.reload_start_time)
		player_reload(level);
	else if (level->ui.state.m1_click && level->ui.state.mouse_capture)
		player_shoot(level);
	if (level->game_logic.win_start_time)
		return ;
	if (*game_state == GAME_STATE_INGAME)
	{
		if (pick_up_pick_ups(level, level->game_logic.ammo_box,
				level->game_logic.ammo_box_amount))
			level->game_logic.player.ammo = PLAYER_AMMO_MAX;
		if (pick_up_pick_ups(level, level->game_logic.health_box,
				level->game_logic.health_box_amount))
			level->game_logic.player.health = PLAYER_HEALTH_MAX;
	}
	else
		reset_pick_ups(level);
	game_logic_win_lose(level, game_state);
}
