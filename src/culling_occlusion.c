/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling_occlusion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/26 04:05:50 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/17 23:58:10 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static int		cull_behind_occlusion(t_vec3 dir, t_vec3 pos, t_tri tri)
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

int				is_value_close(float a, float b, float range)
{
	float diff = a - b;

	return (-range <= diff && diff <= range);
}

float			area(t_vec3 *a, t_vec3 *b, t_vec3 *c)
{
	return (fabs((a->x * (b->y - c->y) + b->x * (c->y - a->y) + c->x * (a->y - b->y)) / 2.0));
}
 
static int		vec3_point_in_tri(t_vec3 *p, t_vec3 *a, t_vec3 *b, t_vec3 *c)
{  
	float A = area(a, b, c);
	float A1 = area(p, b, c);
	float A2 = area(a, p, c);
	float A3 = area(a, b, p);

	return (is_value_close(A, A1 + A2 + A3, OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER));
}
 
static int		point_in_face(t_vec3 *p, t_tri *face)
{
	if (vec3_point_in_tri(p, &face->verts[0].pos, &face->verts[1].pos, &face->verts[2].pos))
		return (1);
	if (face->isquad && vec3_point_in_tri(p, &face->verts[3].pos, &face->verts[2].pos, &face->verts[1].pos))
		return (1);
	return (0);
}

static int		is_bface_in_aface(t_tri a, t_tri b, t_level *level)
{
	int	vert_amount;

	vert_amount = a.isquad ? 4 : 3;
	for (int i = 0; i < vert_amount; i++)
	{
		camera_offset(&a.verts[i].pos, &level->cam);
		if (a.verts[i].pos.z < 0)
			return (0);
	}
	vert_amount = b.isquad ? 4 : 3;
	for (int i = 0; i < vert_amount; i++)
	{
		camera_offset(&b.verts[i].pos, &level->cam);
		if (b.verts[i].pos.z < 0)
			return (0);
	}
	for (int i = 0; i < vert_amount; i++)
		if (!point_in_face(&b.verts[i].pos, &a))
			return (0);
	return (1);
}

int		occlusion_culling(t_tri tri, t_level *level)
{
	for (int i = 0; i < level->visible.tri_amount; i++)
		if (is_bface_in_aface(level->visible.tris[i], tri, level))
			if (!cull_behind_occlusion(level->visible.tris[i].normal, level->visible.tris[i].verts[0].pos, tri))
				return (0);
	return (1);
} 
