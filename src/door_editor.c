/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_editor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 23:29:24 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/06 23:29:24 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	find_selected_door_index(t_level *level)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].selected)
		{
			for (int k = 0; k < level->doors.door_amount; k++)
			{
				for (int m = 0; m < level->doors.door[k].indice_amount; m++)
				{
					if (i == level->doors.door[k].indices[m])
					{
						deselect_all_faces(level);
						level->doors.selected_index = k + 1;
						for (int z = 0; z < level->doors.door[k].indice_amount; z++)
							level->all.tris[level->doors.door[k].indices[z]].selected = TRUE;
						return ;
					}
				}
			}
		}
	}
	if (level->doors.selected_index)
		deselect_all_faces(level);
	level->doors.selected_index = 0;
}

void	door_activate(t_level *level)
{
	int		nearest_index;
	float	len;
	float	nearest_len;

	nearest_index = -1;
	nearest_len = FLT_MAX;;
	for (int i = 0; i < level->doors.door_amount; i++)
	{
		t_vec3 avg = {0, 0, 0};
		t_door *door = &level->doors.door[i];
		int amount = 0;
		for (int j = 0; j < door->indice_amount; j++)
		{
			for (int k = 0; k < 3 + door->isquad[j]; k++)
			{
				vec_add(&avg, avg, level->all.tris[door->indices[j]].verts[k].pos);
				amount++;
			}
		}
		vec_div(&avg, amount);
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
		level->doors.door[nearest_index].transition_direction = level->doors.door[nearest_index].transition_direction ? 0 : 1;
		level->doors.door[nearest_index].transition_start_time = SDL_GetTicks();
	}
}

float	lerp(float a, float b, float f)
{
    return (a + f * (b - a));
}

t_vec3	vec_interpolate(t_vec3 a, t_vec3 b, float f)
{
	t_vec3 res;

	res.x = lerp(a.x, b.x, f);
	res.y = lerp(a.y, b.y, f);
	res.z = lerp(a.z, b.z, f);
	return (res);
}

void	door_animate(t_level *level)
{
	for (int a = 0; a < level->doors.door_amount; a++)
	{
		t_door* door = &level->doors.door[a];
		if (door->transition_start_time)
		{
			float time = (SDL_GetTicks() - door->transition_start_time) / (1000 * door->transition_time);
			for (int i = 0; i < door->indice_amount; i++)
			{
				for (int k = 0; k < 4; k++)
				{
						// level->all.tris[door->indices[i]].verts[k].pos = door->pos1[i][k];
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

void	delete_door(t_level *level)
{
}

void	add_new_door(t_level *level)
{
	level->doors.door_amount++;
	if (!(level->doors.door = (t_door*)realloc(level->doors.door, sizeof(t_door) * level->doors.door_amount)))
		ft_error("memory allocation failed\n");
	if (!(level->doors.door = (t_door*)realloc(level->doors.door, sizeof(t_door) * level->doors.door_amount)))
		ft_error("memory allocation failed\n");
	t_door *door = &level->doors.door[level->doors.door_amount - 1];
	ft_bzero(door, sizeof(t_door));
	int selected = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
		if (level->all.tris[i].selected)
			selected++;
	door->indice_amount = selected;
	if (!(door->isquad = (int*)malloc(sizeof(int) * selected)))
		ft_error("memory allocation failed\n");
	if (!(door->indices = (int*)malloc(sizeof(int) * selected)))
		ft_error("memory allocation failed\n");
	if (!(door->pos1 = (t_vec3**)malloc(sizeof(t_vec3*) * selected)))
		ft_error("memory allocation failed\n");
	if (!(door->pos2 = (t_vec3**)malloc(sizeof(t_vec3*) * selected)))
		ft_error("memory allocation failed\n");
	selected = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].selected)
		{
			door->isquad[selected] = level->all.tris[i].isquad;
			door->indices[selected] = i;
			if (!(door->pos1[selected] = (t_vec3*)malloc(sizeof(t_vec3) * 4)))
				ft_error("memory allocation failed\n");
			if (!(door->pos2[selected] = (t_vec3*)malloc(sizeof(t_vec3) * 4)))
				ft_error("memory allocation failed\n");
			for (int k = 0; k < 4; k++)
			{
				door->pos1[selected][k] = level->all.tris[i].verts[k].pos;
				door->pos2[selected][k] = level->all.tris[i].verts[k].pos;
			}
			selected++;
		}
	}
}

void	set_door_pos_1(t_level *level)
{
	t_door* door = &level->doors.door[level->doors.selected_index - 1];

	for (int i = 0; i < door->indice_amount; i++)
		for (int k = 0; k < 4; k++)
			door->pos1[i][k] = level->all.tris[door->indices[i]].verts[k].pos;
}

void	set_door_pos_2(t_level *level)
{
	t_door* door = &level->doors.door[level->doors.selected_index - 1];

	for (int i = 0; i < door->indice_amount; i++)
		for (int k = 0; k < 4; k++)
			door->pos2[i][k] = level->all.tris[door->indices[i]].verts[k].pos;
}

void	enable_door_editor(t_level *level)
{
	level->ui.state.ui_location = UI_LOCATION_DOOR_EDITOR;
	level->ui.wireframe = TRUE;
	level->ui.wireframe_on_top = TRUE;
	level->ui.wireframe_culling_visual = FALSE;
	level->ui.vertex_select_mode = FALSE;
}
