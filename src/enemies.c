/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 17:08:49 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/12 11:30:19 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	delete_enemy(t_level *level)
{
	int	amount;
	int	i;

	amount = level->game_logic.enemy_amount;
	i = level->ui.state.logic_selected_index;
	while (i < amount - 1)
	{
		level->game_logic.enemies[i].spawn_pos
			= level->game_logic.enemies[i + 1].spawn_pos;
		i++;
	}
	level->game_logic.enemies
		= (t_enemy *)ft_realloc(level->game_logic.enemies,
			sizeof(t_enemy) * amount, sizeof(t_enemy) * (amount - 1));
	if (!level->game_logic.enemies)
		ft_error("memory allocation failed");
	level->game_logic.enemy_amount--;
	level->ui.state.logic_selected = GAME_LOGIC_SELECTED_NONE;
}

void	add_enemy(t_level *level)
{
	int	amount;

	amount = level->game_logic.enemy_amount;
	level->game_logic.enemies
		= (t_enemy *)ft_realloc(level->game_logic.enemies,
			sizeof(t_enemy) * amount, sizeof(t_enemy) * (amount + 1));
	if (!level->game_logic.enemies)
		ft_error("memory allocation failed");
	vec_add(&level->game_logic.enemies[amount].spawn_pos,
		level->cam.pos, level->cam.front);
	obj_pos_set_to_floor(&level->game_logic.enemies[amount].spawn_pos,
		&level->game_models.enemy, level);
	level->game_logic.enemy_amount++;
}

static void	enemy_spawn(t_enemy *enemy, t_enemy_settings *settings)
{
	enemy->dead_start_time = 0;
	enemy->current_attack_delay = 0;
	enemy->dir = (t_vec3){1, 0, 0};
	enemy->dir_rad = 0;
	enemy->pos = enemy->spawn_pos;
	enemy->remaining_health = settings->initial_health;
	enemy->move_start_time = SDL_GetTicks()
		+ (rand() % (int)(1000 * settings->move_duration));
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
	int				i;

	i = -1;
	while (++i < level->game_logic.enemy_amount)
		if (!level->game_logic.enemies[i].dead_start_time)
			enemy_state_machine(&level->game_logic.enemies[i], level);
}
