/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 13:54:47 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	deselect_all_faces(t_level *level)
{
	int	i;
	int	o;

	i = 0;
	while (i < level->all.tri_amount)
	{
		o = 0;
		while (o < 3 + level->all.tris[i].isquad)
		{
			level->all.tris[i].verts[o].selected = FALSE;
			o++;
		}
		level->all.tris[i].selected = FALSE;
		i++;
	}
}

void	find_closest_mouse(t_vec3 *vert, int *i, int *k, t_ivec2 *mouse)
{
	static float	nearest_len = -1;
	static int		nearest_tri_index = -1;
	static int		nearest_vert_index = -1;

	if (!vert)
	{
		*k = nearest_vert_index;
		*i = nearest_tri_index;
		nearest_vert_index = -1;
		nearest_tri_index = -1;
		nearest_len = -1;
	}
	if (!vert || vert->z < 0)
		return ;
	if (vec2_length((t_vec2){vert->x - mouse->x, vert->y - mouse->y})
		< nearest_len || nearest_len == -1)
	{
		nearest_len
			= vec2_length((t_vec2){vert->x - mouse->x, vert->y - mouse->y});
		nearest_tri_index = *i;
		nearest_vert_index = *k;
	}
}

void	update_closest_vertex(t_level *level)
{
	int		i;
	int		k;
	int		new_state;
	t_ivec2	o;
	t_vec3	pos;

	find_closest_mouse(NULL, &i, &k, NULL);
	if (level->ui.state.m1_click && i != -1
		&& level->ui.state.mouse_location == MOUSE_LOCATION_SELECTION)
	{
		new_state = level->all.tris[i].verts[k].selected == 0;
		pos = level->all.tris[i].verts[k].pos;
		o.x = 0;
		while (o.x < level->all.tri_amount)
		{
			o.y = 0;
			while (o.y < 3 + level->all.tris[o.x].isquad)
			{
				if (vec_cmp(level->all.tris[o.x].verts[o.y].pos, pos))
					level->all.tris[o.x].verts[o.y].selected = new_state;
				o.y++;
			}
			o.x++;
		}
	}
}

void	raycast_face_selection(t_ray vec, t_level *level)
{
	float	dist;
	float	tmp;
	int		hit;
	int		i;

	i = 0;
	hit = 0;
	dist = FLT_MAX;
	while (i < level->all.tri_amount)
	{
		tmp = cast_face(level->all.tris[i], vec, NULL);
		if (tmp > 0 && tmp < dist)
		{
			dist = tmp;
			hit = i;
		}
		i++;
	}
	if (dist != FLT_MAX)
		level->all.tris[hit].selected = level->all.tris[hit].selected == 0;
	else
		deselect_all_faces(level);
}

void	select_face(t_camera *cam, t_level *level)
{
	t_ray	r;
	float	xm;
	float	ym;

	if (level->ui.vertex_select_mode)
		update_closest_vertex(level);
	else if (level->ui.state.m1_click)
	{
		r.dir = cam->front;
		r.pos = cam->pos;
		ym = cam->fov_y / RES_Y * level->ui.state.mouse.y - cam->fov_y / 2;
		xm = cam->fov_x / RES_X * level->ui.state.mouse.x - cam->fov_x / 2;
		r.dir.x += cam->up.x * ym + cam->side.x * xm;
		r.dir.y += cam->up.y * ym + cam->side.y * xm;
		r.dir.z += cam->up.z * ym + cam->side.z * xm;
		raycast_face_selection(r, level);
	}
}

void	toggle_selection_all(t_level *level)
{
	int	selected;
	int	i;
	int	j;

	i = 0;
	selected = FALSE;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
			selected = TRUE;
		i++;
	}
	i = 0;
	while (i < level->all.tri_amount)
	{
		level->all.tris[i].selected = !selected;
		j = 0;
		while (j < 3 + level->all.tris[i].isquad)
		{
			level->all.tris[i].verts[j].selected = !selected;
			j++;
		}
		i++;
	}
}
