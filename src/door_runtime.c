/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_runtime.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/10 19:12:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/10 19:43:16 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	door_activate(t_level *level)
{
	int		nearest_index;
	float	len;
	float	nearest_len;
	t_door	*door;
	t_vec3	avg;
	int		amount;

	nearest_index = -1;
	nearest_len = FLT_MAX;
	for (int i = 0; i < level->doors.door_amount; i++)
	{
		ft_memset(&avg, 0, sizeof(t_vec3));
		door = &level->doors.door[i];
		if (door->is_activation_pos_active)
			avg = door->activation_pos;
		else
		{
			amount = 0;
			for (int j = 0; j < door->indice_amount; j++)
			{
				for (int k = 0; k < 3 + door->isquad[j]; k++)
				{
					vec_add(&avg, avg, level->all.tris[door->indices[j]].verts[k].pos);
					amount++;
				}
			}
			vec_div(&avg, amount);
		}
		vec_sub(&avg, avg, level->cam.pos);
		len = vec_length(avg);
		if (len < nearest_len)
		{
			nearest_index = i;
			nearest_len = len;
		}
	}
	if (nearest_index != -1 && nearest_len < DOOR_ACTIVATION_DISTANCE)
	{
		Mix_PlayChannel(AUDIO_DOOR_CHANNEL, level->audio.door, 0);
		door = &level->doors.door[nearest_index];
		door->transition_direction = door->transition_direction ? 0 : 1;
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
}

void	door_animate(t_level *level)
{
	t_door	*door;
	float	time;

	for (int a = 0; a < level->doors.door_amount; a++)
	{
		door = &level->doors.door[a];
		if (door->transition_start_time)
		{
			time = (SDL_GetTicks() - door->transition_start_time)
				/ (1000 * door->transition_time);
			for (int i = 0; i < door->indice_amount; i++)
			{
				for (int k = 0; k < 3 + door->isquad[i]; k++)
				{
					if (time < 1)
					{
						if (door->transition_direction)
							level->all.tris[door->indices[i]].verts[k].pos = vec_interpolate(door->pos2[i][k], door->pos1[i][k], time);
						else
							level->all.tris[door->indices[i]].verts[k].pos = vec_interpolate(door->pos1[i][k], door->pos2[i][k], time);
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
			}
		}
	}
}
