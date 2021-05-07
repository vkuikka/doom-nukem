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
	for (int i = 0; i < level->doors.door_amount; i++)
	{
		level->doors.door[i].transition_direction = level->doors.door[i].transition_direction ? 0 : 1;
		level->doors.door[i].transition_start_time = SDL_GetTicks();
	}
	// if (level->doors.selected_index)
	//find closest door
	//if closest distance < DOOR_ACTIVATION_DISTANCE
}

void	door_animate(t_level *level)
{
	if (!level->doors.selected_index)
		return ;
	t_door* door = &level->doors.door[level->doors.selected_index - 1];

	if (door->transition_start_time)
	{
		for (int i = 0; i < door->indice_amount; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (level->doors.door[i].transition_direction)
					level->all.tris[door->indices[i]].verts[k].pos = door->pos1[i][k];
				else
					level->all.tris[door->indices[i]].verts[k].pos = door->pos2[i][k];
			}
		}
		door->transition_start_time = 0;
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
