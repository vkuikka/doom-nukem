/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_editor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 23:29:24 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/03 04:05:03 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	select_door(int index, t_level *level)
{
	int	i;

	deselect_all_faces(level);
	level->doors.selected_index = index + 1;
	i = -1;
	while (++i < level->doors.door[index].indice_amount)
		level->all.tris[level->doors.door[index].indices[i]]
			.selected = TRUE;
}

void	deselect_door(t_level *level)
{
	if (level->doors.selected_index)
		deselect_all_faces(level);
	level->doors.selected_index = 0;
}

void	find_selected_door_index(t_level *level)
{
	int	i;
	int	k;
	int	m;

	i = -1;
	while (++i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			k = -1;
			while (++k < level->doors.door_amount)
			{
				m = -1;
				while (++m < level->doors.door[k].indice_amount)
				{
					if (i == level->doors.door[k].indices[m])
						return (select_door(k, level));
				}
			}
		}
	}
	deselect_door(level);
}

void	door_find_avg(t_level *level)
{
	t_ivec3	i;
	t_door	*door;
	int		avg_amount;

	i.x = -1;
	while (++i.x < level->doors.door_amount)
	{
		door = &level->doors.door[i.x];
		vec_mult(&door->avg, 0);
		avg_amount = 0;
		i.y = -1;
		while (++i.y < door->indice_amount)
		{
			i.z = -1;
			while (++i.z < 3 + door->isquad[i.y])
			{
				vec_add(&door->avg, door->avg,
					level->all.tris[door->indices[i.y]].verts[i.z].pos);
				avg_amount++;
			}
		}
		if (avg_amount)
			vec_div(&door->avg, avg_amount);
	}
}

void	door_put_text(t_level *level)
{
	t_door	*door;
	char	buf[100];
	int		i;

	door_find_avg(level);
	i = 0;
	while (i < level->doors.door_amount)
	{
		door = &level->doors.door[i];
		sprintf(buf, "door %d", i + 1);
		render_text_3d(buf, door->avg, DOOR_LOCATION_INFO_COLOR, level);
		if (door->is_activation_pos_active)
		{
			sprintf(buf, "door %d activation", i + 1);
			render_text_3d(buf, door->activation_pos,
				DOOR_ACTIVATION_LOCATION_INFO_COLOR, level);
		}
		i++;
	}
}

void	door_activation_move(t_level *level, t_vec3 move_amount)
{
	int	index;

	index = level->doors.selected_index - 1;
	vec_add(&level->doors.door[index].activation_pos,
		level->doors.door[index].activation_pos, move_amount);
	level->ui.state.gizmo_pos = level->doors.door[index].activation_pos;
}

void	delete_all_doors(t_level *level)
{
	t_door	*door;
	int		i;
	int		k;

	k = -1;
	while (++k < level->doors.door_amount)
	{
		door = &level->doors.door[k];
		i = 0;
		while (i < door->indice_amount)
		{
			free(door->pos1[i]);
			free(door->pos2[i]);
			i++;
		}
		free(door->pos1);
		free(door->pos2);
		free(door->indices);
		free(door->isquad);
	}
	level->doors.door_amount = 0;
	free(level->doors.door);
	level->doors.door = NULL;
	deselect_all_faces(level);
	level->doors.selected_index = 0;
}

void	delete_door(t_level *level)
{
	t_door	*door;
	int		i;

	door = &level->doors.door[level->doors.selected_index - 1];
	i = 0;
	while (i < door->indice_amount)
	{
		free(door->pos1[i]);
		free(door->pos2[i]);
		i++;
	}
	free(door->pos1);
	free(door->pos2);
	free(door->indices);
	free(door->isquad);
	i = level->doors.selected_index - 2;
	while (++i < level->doors.door_amount - 1)
		level->doors.door[i] = level->doors.door[i + 1];
	level->doors.door_amount--;
	level->doors.door = (t_door *)ft_realloc(level->doors.door,
			sizeof(t_door) * level->doors.door_amount - 1,
			sizeof(t_door) * level->doors.door_amount);
	if (!level->doors.door)
		ft_error("memory allocation failed\n");
	deselect_door(level);
}

t_door	*alloc_new_door(t_level *level)
{
	t_door	*door;
	int		selected;
	int		i;

	level->doors.door_amount++;
	level->doors.door = (t_door *)ft_realloc(level->doors.door,
			sizeof(t_door) * level->doors.door_amount,
			sizeof(t_door) * level->doors.door_amount + 1);
	if (!level->doors.door)
		ft_error("memory allocation failed\n");
	door = &level->doors.door[level->doors.door_amount - 1];
	ft_bzero(door, sizeof(t_door));
	selected = 0;
	i = -1;
	while (++i < level->all.tri_amount)
		if (level->all.tris[i].selected)
			selected++;
	door->indice_amount = selected;
	door->isquad = (int *)malloc(sizeof(int) * selected);
	door->indices = (int *)malloc(sizeof(int) * selected);
	door->pos1 = (t_vec3 **)malloc(sizeof(t_vec3 *) * selected);
	door->pos2 = (t_vec3 **)malloc(sizeof(t_vec3 *) * selected);
	if (!door->pos2 || !door->pos1 || !door->indices || !door->isquad)
		ft_error("memory allocation failed\n");
	return (door);
}

void	add_new_door(t_level *level)
{
	t_door	*door;
	t_ivec3	i;

	door = alloc_new_door(level);
	i.z = 0;
	i.x = -1;
	while (++i.x < level->all.tri_amount)
	{
		if (level->all.tris[i.x].selected)
		{
			door->isquad[i.z] = level->all.tris[i.x].isquad;
			door->indices[i.z] = i.x;
			door->pos1[i.z] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
			door->pos2[i.z] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
			if (!door->pos2[i.z] || !door->pos1[i.z])
				ft_error("memory allocation failed\n");
			i.y = -1;
			while (++i.y < 4)
			{
				door->pos1[i.z][i.y] = level->all.tris[i.x].verts[i.y].pos;
				door->pos2[i.z][i.y] = level->all.tris[i.x].verts[i.y].pos;
			}
			i.z++;
		}
	}
}

static void	set_door_pos(t_level *level, int is_pos2)
{
	t_door	*door;
	int		i;
	int		k;

	door = &level->doors.door[level->doors.selected_index - 1];
	i = 0;
	while (i < door->indice_amount)
	{
		k = 0;
		while (k < 4)
		{
			if (is_pos2)
				door->pos2[i][k]
					= level->all.tris[door->indices[i]].verts[k].pos;
			else
				door->pos1[i][k]
					= level->all.tris[door->indices[i]].verts[k].pos;
			k++;
		}
		i++;
	}
}

void	set_door_pos_1(t_level *level)
{
	set_door_pos(level, 0);
}

void	set_door_pos_2(t_level *level)
{
	set_door_pos(level, 1);
}

void	enable_door_editor(t_level *level)
{
	level->ui.state.ui_location = UI_LOCATION_DOOR_EDITOR;
	level->ui.wireframe = TRUE;
	level->ui.wireframe_on_top = TRUE;
	level->ui.wireframe_culling_visual = FALSE;
	level->ui.vertex_select_mode = FALSE;
}
