/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_runtime_0.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	door_start_animate(t_door *door)
{
	door->transition_direction = door->transition_direction == 0;
	if ((SDL_GetTicks() - door->transition_start_time)
		/ (1000 * door->transition_time) < 1)
	{
		door->transition_start_time = SDL_GetTicks()
			- ((door->transition_time * 1000)
				- (SDL_GetTicks() - door->transition_start_time));
	}
	else
		door->transition_start_time = SDL_GetTicks();
}

float	dist_to_door_activation(t_level *level, t_door *door)
{
	int		amount;
	t_vec3	avg;
	int		j;
	int		k;

	if (door->is_activation_pos_active)
	{
		vec_sub(&avg, door->activation_pos, level->cam.pos);
		return (vec_length(avg));
	}
	avg = (t_vec3){0, 0, 0};
	amount = 0;
	j = -1;
	while (++j < door->indice_amount)
	{
		k = -1;
		while (++k < 3 + door->isquad[j])
		{
			vec_add(&avg, avg, level->all.tris[door->indices[j]].verts[k].pos);
			amount++;
		}
	}
	vec_div(&avg, amount);
	vec_sub(&avg, avg, level->cam.pos);
	return (vec_length(avg));
}

void	door_activate(t_level *level)
{
	int		nearest_index;
	float	len;
	float	nearest_len;
	int		i;

	nearest_index = -1;
	nearest_len = FLT_MAX;
	i = -1;
	while (++i < level->doors.door_amount)
	{
		len = dist_to_door_activation(level, &level->doors.door[i]);
		if (len < nearest_len)
		{
			nearest_index = i;
			nearest_len = len;
		}
	}
	if (nearest_index != -1 && nearest_len < DOOR_ACTIVATION_DISTANCE)
	{
		Mix_PlayChannel(AUDIO_DOOR_CHANNEL, level->audio.door, 0);
		door_start_animate(&level->doors.door[nearest_index]);
	}
}

void	door_move_vert(t_level *level, t_door *door, float time, t_ivec2 v)
{
	int	i;
	int	k;

	i = v.x;
	k = v.y;
	if (time < 1)
	{
		if (door->transition_direction)
			level->all.tris[door->indices[i]].verts[k].pos
				= vec_interpolate(door->pos2[i][k], door->pos1[i][k], time);
		else
			level->all.tris[door->indices[i]].verts[k].pos
				= vec_interpolate(door->pos1[i][k], door->pos2[i][k], time);
	}
	else
	{
		if (door->transition_direction)
			level->all.tris[door->indices[i]].verts[k].pos = door->pos1[i][k];
		else
			level->all.tris[door->indices[i]].verts[k].pos = door->pos2[i][k];
		door->transition_start_time = 0;
	}
}

void	door_animate(t_level *level)
{
	t_door	*door;
	float	time;
	int		a;
	int		i;
	int		k;

	a = -1;
	while (++a < level->doors.door_amount)
	{
		door = &level->doors.door[a];
		if (door->transition_start_time)
		{
			time = (SDL_GetTicks() - door->transition_start_time)
				/ (1000 * door->transition_time);
			i = -1;
			while (++i < door->indice_amount)
			{
				k = -1;
				while (++k < 3 + door->isquad[i])
					door_move_vert(level, door, time, (t_ivec2){i, k});
			}
		}
	}
}
