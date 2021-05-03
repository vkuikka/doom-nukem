/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 18:32:46 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/18 17:14:22 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	deselect_all_faces(t_level *level)
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

void			find_closest_mouse(t_vec3 *vert, int *i, int *k)
{
	static float	nearest_len = -1;
	static int		nearest_tri_index = -1;
	static int		nearest_vert_index = -1;
	float			len;
	t_vec2			test;
	int				x;
	int				y;

	if (!vert)
	{
		*k = nearest_vert_index;
		*i = nearest_tri_index;
		nearest_vert_index = -1;
		nearest_tri_index = -1;
		nearest_len = -1;
		return ;
	}
	if (vert->z < 0)
		return ;
	SDL_GetMouseState(&x, &y);
	test.x = vert->x;
	test.y = vert->y;
	test.x -= x;
	test.y -= y;
	len = vec2_length(test);
	if (len < nearest_len || nearest_len == -1)
	{
		nearest_len = len;
		nearest_tri_index = *i;
		nearest_vert_index = *k;
	}
}

static void		update_closest_vertex(t_level *level)
{
	int	i;
	int	k;

	find_closest_mouse(NULL, &i, &k);
	if (level->ui.state.m1_click && i != -1 && level->ui.state.mouse_location == MOUSE_LOCATION_SELECTION)
	{
		int new_state = level->all.tris[i].verts[k].selected ? 0 : 1;
		t_vec3 pos = level->all.tris[i].verts[k].pos;
		for (int o = 0; o < level->all.tri_amount; o++)
			for (int p = 0; p < 3 + level->all.tris[o].isquad; p++)
				if (vec_cmp(level->all.tris[o].verts[p].pos, pos))
					level->all.tris[o].verts[p].selected = new_state;
	}
}

static void	raycast_face_selection(t_ray vec, t_level *level)
{
	float	dist;
	float	tmp;
	int		hit;
	int		i;

	i = 0;
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
		level->all.tris[hit].selected = level->all.tris[hit].selected ? 0 : 1;
	else
		deselect_all_faces(level);
}

void	select_face(t_camera *cam, t_level *level, int x, int y)
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
		ym = cam->fov_y / RES_Y * y - cam->fov_y / 2;
		xm = cam->fov_x / RES_X * x - cam->fov_x / 2;
		r.dir.x += cam->up.x * ym + cam->side.x * xm;
		r.dir.y += cam->up.y * ym + cam->side.y * xm;
		r.dir.z += cam->up.z * ym + cam->side.z * xm;
		raycast_face_selection(r, level);
	}
}
