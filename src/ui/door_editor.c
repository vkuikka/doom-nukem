/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_editor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 23:29:24 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/13 17:16:11 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	find_selected_door_index(t_level *level)
{
	int	i;
	int	k;
	int	m;
	int	z;

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
					{
						deselect_all_faces(level);
						level->doors.selected_index = k + 1;
						z = -1;
						while (++z < level->doors.door[k].indice_amount)
							level->all.tris[level->doors.door[k].indices[z]]
								.selected = TRUE;
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

void	door_put_text(t_window *window, t_level *level)
{
	t_vec3	avg;
	t_door	*door;
	int		avg_amount;
	char	buf[100];
	int		a;
	int		i;
	int		k;

	a = -1;
	while (++a < level->doors.door_amount)
	{
		avg.x = 0;
		avg.y = 0;
		avg.z = 0;
		avg_amount = 0;
		door = &level->doors.door[a];
		i = -1;
		while (++i < door->indice_amount)
		{
			k = -1;
			while (++k < 3 + door->isquad[i])
			{
				vec_add(&avg, avg,
					level->all.tris[door->indices[i]].verts[k].pos);
				avg_amount++;
			}
		}
		if (avg_amount)
		{
			vec_div(&avg, avg_amount);
			camera_offset(&avg, &level->cam);
			if (avg.z > 0)
			{
				set_text_color(DOOR_LOCATION_INFO_COLOR);
				sprintf(buf, "door %d", a + 1);
				render_text(buf, avg.x, avg.y);
			}
		}
		if (door->is_activation_pos_active)
		{
			avg = door->activation_pos;
			camera_offset(&avg, &level->cam);
			if (avg.z > 0)
			{
				set_text_color(DOOR_ACTIVATION_LOCATION_INFO_COLOR);
				sprintf(buf, "door %d activation", a + 1);
				render_text(buf, avg.x, avg.y);
			}
		}
	}
}

//gizmo calls this
void	door_activation_move(t_level *level, t_vec3 move_amount)
{
	int	index;

	index = level->doors.selected_index - 1;
	vec_add(&level->doors.door[index].activation_pos,
		level->doors.door[index].activation_pos, move_amount);
	level->ui.state.gizmo_pos = level->doors.door[index].activation_pos;
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
	i = level->doors.selected_index - 1;
	while (i < level->doors.door_amount - 1)
	{
		level->doors.door[i] = level->doors.door[i + 1];
		i++;
	}
	level->doors.door_amount--;
	level->doors.door = (t_door *)ft_realloc(level->doors.door,
			sizeof(t_door) * level->doors.door_amount - 1,
			sizeof(t_door) * level->doors.door_amount);
	if (!level->doors.door)
		ft_error("memory allocation failed\n");
}

void	add_new_door(t_level *level)
{
	t_door	*door;
	int		selected;
	int		i;
	int		k;

	level->doors.door_amount++;
	level->doors.door = (t_door *)ft_realloc(level->doors.door,
			sizeof(t_door) * level->doors.door_amount - 1,
			sizeof(t_door) * level->doors.door_amount);
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
	if (!door->isquad)
		ft_error("memory allocation failed\n");
	door->indices = (int *)malloc(sizeof(int) * selected);
	if (!door->indices)
		ft_error("memory allocation failed\n");
	door->pos1 = (t_vec3 **)malloc(sizeof(t_vec3 *) * selected);
	if (!door->pos1)
		ft_error("memory allocation failed\n");
	door->pos2 = (t_vec3 **)malloc(sizeof(t_vec3 *) * selected);
	if (!door->pos2)
		ft_error("memory allocation failed\n");
	selected = 0;
	i = -1;
	while (++i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			door->isquad[selected] = level->all.tris[i].isquad;
			door->indices[selected] = i;
			door->pos1[selected] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
			if (!door->pos1[selected])
				ft_error("memory allocation failed\n");
			door->pos2[selected] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
			if (!door->pos2[selected])
				ft_error("memory allocation failed\n");
			k = -1;
			while (++k < 4)
			{
				door->pos1[selected][k] = level->all.tris[i].verts[k].pos;
				door->pos2[selected][k] = level->all.tris[i].verts[k].pos;
			}
			selected++;
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
			door->pos2[i][k] = level->all.tris[door->indices[i]].verts[k].pos;
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
