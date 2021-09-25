/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/10 22:47:18 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/25 16:32:15 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	player_reload(t_level *level)
{
	float	time;

	time = (SDL_GetTicks() - level->game_logic.reload_start_time)
		/ (1000 * VIEWMODEL_ANIM_FPS);
	if (time > 1)
	{
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

static int	pick_up_pick_ups(t_level *level, t_item_pickup *pickups, int amount)
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

static void	reset_pick_ups(t_level *level)
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

static void	game_logic_win_lose(t_level *level, t_game_state *game_state)
{
	t_vec3	dist;

	if (level->game_logic.player_health <= 0)
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

void	game_logic(t_level *level, t_game_state *game_state)
{
	handle_audio(level, game_state);
	if (level->game_logic.win_start_time || level->game_logic.death_start_time)
	{
		game_finished(level, game_state, 0);
		return ;
	}
	if (level->game_logic.reload_start_time && *game_state != GAME_STATE_DEAD)
		player_reload(level);
	if (level->ui.state.m1_click && level->ui.state.mouse_capture
		&& *game_state != GAME_STATE_DEAD)
		player_shoot(level);
	if (*game_state == GAME_STATE_INGAME)
	{
		if (pick_up_pick_ups(level, level->game_logic.ammo_box,
				level->game_logic.ammo_box_amount))
			level->game_logic.player_ammo = PLAYER_AMMO_MAX;
		if (pick_up_pick_ups(level, level->game_logic.health_box,
				level->game_logic.health_box_amount))
			level->game_logic.player_health = PLAYER_HEALTH_MAX;
	}
	else
		reset_pick_ups(level);
	game_logic_win_lose(level, game_state);
}
