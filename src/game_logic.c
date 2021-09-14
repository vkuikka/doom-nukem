/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/10 22:47:18 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/14 18:50:26 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	player_reload(t_level *level)
{
	float	time;

	time = (SDL_GetTicks() - level->reload_start_time)
		/ (1000 * VIEWMODEL_ANIM_FPS);
	if (time < 1)
		level->viewmodel_index = (int)(time * VIEWMODEL_FRAMES);
	else
	{
		level->viewmodel_index = 0;
		level->reload_start_time = 0;
		level->player_ammo = PLAYER_AMMO_MAX;
	}
}

static void	player_shoot(t_level *level)
{
	if (level->player_ammo)
	{
		level->player_ammo--;
		level->player.dir = level->cam.front;
		create_projectile(level, level->cam.pos,
			level->cam.front, &level->player);
	}
	else
		level->reload_start_time = SDL_GetTicks();
}

static void	game_finished(t_level *level, t_game_state *game_state,
				float time)
{
	if (level->win_start_time)
	{
		time = (SDL_GetTicks() - level->win_start_time)
			/ (1000.0 * WIN_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->win_start_time = 0;
		}
	}
	else if (level->death_start_time)
	{
		level->player_health = 0;
		time = (SDL_GetTicks() - level->death_start_time)
			/ (1000.0 * DEATH_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->death_start_time = 0;
		}
	}
}

void	game_logic_put_info(t_level *level, unsigned int *texture)
{
	t_vec3	a;
	t_vec3	b;
	t_vec3	dist;

	render_text_3d("spawn", level->spawn_pos.pos,
		UI_LEVEL_SETTINGS_TEXT_COLOR, level);
	render_text_3d("menu1", level->main_menu_pos1.pos,
		UI_LEVEL_SETTINGS_TEXT_COLOR, level);
	render_text_3d("menu2", level->main_menu_pos2.pos,
		UI_LEVEL_SETTINGS_TEXT_COLOR, level);
	a = level->main_menu_pos1.pos;
	b = level->main_menu_pos2.pos;
	camera_offset(&a, &level->cam);
	camera_offset(&b, &level->cam);
	print_line(a, b, UI_LEVEL_SETTINGS_TEXT_COLOR, texture);
	vec_sub(&dist, level->win_pos, level->cam.pos);
	if (vec_length(dist) < level->win_dist)
		render_text_3d("win", level->win_pos, UI_LEVEL_BAKED_COLOR, level);
	else
		render_text_3d("win", level->win_pos, UI_LEVEL_NOT_BAKED_COLOR, level);
}

void	game_logic(t_level *level, t_game_state *game_state)
{
	t_vec3	dist;

	handle_audio(level, game_state);
	if (level->reload_start_time && *game_state != GAME_STATE_DEAD)
		player_reload(level);
	if (level->ui.state.m1_click && level->ui.state.mouse_capture
		&& *game_state != GAME_STATE_DEAD)
		player_shoot(level);
	if (level->win_start_time || level->death_start_time)
		game_finished(level, game_state, 0);
	else if (level->player_health <= 0)
	{
		level->reload_start_time = 0;
		level->death_start_time = SDL_GetTicks();
		*game_state = GAME_STATE_DEAD;
		level->cam.look_up = 1;
	}
	else if (*game_state == GAME_STATE_INGAME)
	{
		vec_sub(&dist, level->win_pos, level->cam.pos);
		if (vec_length(dist) < level->win_dist)
			*game_state = GAME_STATE_WIN;
		if (*game_state == GAME_STATE_WIN)
			level->win_start_time = SDL_GetTicks();
	}
}
