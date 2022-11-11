/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling_occlusion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 15:02:17 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

int	cull_behind_occlusion(t_vec3 dir, t_vec3 pos, t_tri tri)
{
	t_vec3	vert;
	int		i;

	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&vert, tri.verts[i].pos, pos);
		if (vec_dot(dir, vert) >= 0)
			return (TRUE);
		i++;
	}
	return (FALSE);
}

int	is_value_close(float a, float b, float range)
{
	float	diff;

	diff = a - b;
	return (-range <= diff && diff <= range);
}

float	area(t_vec3 *a, t_vec3 *b, t_vec3 *c)
{
	return (fabs(
			(a->x * (b->y - c->y) + b->x * (c->y - a->y) + c->x * (a->y - b->y))
			/ 2.0));
}

int	vec3_point_in_tri(t_vec3 *p, t_vec3 *a, t_vec3 *b, t_vec3 *c)
{
	float	a0;
	float	a1;
	float	a2;
	float	a3;

	a0 = area(a, b, c);
	a1 = area(p, b, c);
	a2 = area(a, p, c);
	a3 = area(a, b, p);
	return (is_value_close(
			a0, a1 + a2 + a3, OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER));
}

int	vec3_point_in_face(t_vec3 *p, t_tri *face)
{
	if (vec3_point_in_tri(p,
			&face->verts[0].pos, &face->verts[1].pos, &face->verts[2].pos))
		return (1);
	if (face->isquad
		&& vec3_point_in_tri(p,
			&face->verts[3].pos, &face->verts[2].pos, &face->verts[1].pos))
		return (1);
	return (0);
}

int	is_bface_in_aface(t_tri a, t_tri b, t_level *level)
{
	int	vert_amount;
	int	i;

	vert_amount = 3 + a.isquad;
	i = -1;
	while (++i < vert_amount)
	{
		camera_offset(&a.verts[i].pos, &level->cam);
		if (a.verts[i].pos.z < 0)
			return (0);
	}
	vert_amount = 3 + b.isquad;
	i = -1;
	while (++i < vert_amount)
	{
		camera_offset(&b.verts[i].pos, &level->cam);
		if (b.verts[i].pos.z < 0)
			return (0);
	}
	i = -1;
	while (++i < vert_amount)
		if (!vec3_point_in_face(&b.verts[i].pos, &a))
			return (0);
	return (1);
}

int	occlusion_culling(t_tri tri, t_level *level)
{
	int	i;

	i = 0;
	while (i < level->visible.tri_amount)
	{
		if (!level->visible.tris[i].opacity)
			if (is_bface_in_aface(level->visible.tris[i], tri, level))
				if (!cull_behind_occlusion(level->visible.tris[i].normal,
						level->visible.tris[i].verts[0].pos, tri))
					return (0);
		i++;
	}
	return (1);
}
