/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling_occlusion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/26 04:05:50 by rpehkone          #+#    #+#             */
/*   Updated: 2021/03/31 17:41:37 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

int				is_value_close(float a, float b, float range)
{
	float diff = a - b;

	return (-range <= diff && diff <= range);
}

float			area(t_vec3 *a, t_vec3 *b, t_vec3 *c)
{
	return (abs((a->x * (b->y - c->y) + b->x * (c->y - a->y) + c->x * (a->y - b->y)) / 2.0));
}
 
int				point_in_tri(t_vec3 *p, t_vec3 *a, t_vec3 *b, t_vec3 *c)
{  
	float A = area(a, b, c);
	float A1 = area(p, b, c);
	float A2 = area(a, p, c);
	float A3 = area(a, b, p);

	// printf("A = %f\n", A);
	// return (A == A1 + A2 + A3);
	return (is_value_close(A, A1 + A2 + A3, OCCLUSION_CULLING_FLOAT_ERROR_MAGIC_NUMBER));
}
 
static int		point_in_face(t_vec3 *p, t_tri *face)
{
	if (point_in_tri(p, &face->verts[0].pos, &face->verts[1].pos, &face->verts[2].pos))
		return (1);
	if (face->isquad && point_in_tri(p, &face->verts[3].pos, &face->verts[2].pos, &face->verts[1].pos))
		return (1);
	return (0);
}

static int		is_bface_in_aface(t_tri a, t_tri b, t_level *level)
{
	int	vert_amount;

	vert_amount = a.isquad ? 4 : 3;
	for (int i = 0; i < vert_amount; i++)
	{
		camera_offset(&a.verts[i].pos, level);
		if (a.verts[i].pos.z < 0)
			return (0);
	}
	vert_amount = b.isquad ? 4 : 3;
	for (int i = 0; i < vert_amount; i++)
	{
		camera_offset(&b.verts[i].pos, level);
		if (b.verts[i].pos.z < 0)
			return (0);
	}
	for (int i = 0; i < vert_amount; i++)
		if (!point_in_face(&b.verts[i].pos, &a))
			return (0);
	return (1);
}

static int		is_aface_closer(t_tri a, t_tri b, t_level *level)
{
	float	a_far;
	int		vert_amount;

	a_far = 0;
	vert_amount = a.isquad ? 4 : 3;
	for (int i = 0; i < vert_amount; i++)
	{
		//vec sub
		a.verts[i].pos.x -= level->pos.x;
		a.verts[i].pos.y -= level->pos.y;
		a.verts[i].pos.z -= level->pos.z;
		if (a_far < vec_length(a.verts[i].pos))
			a_far = vec_length(a.verts[i].pos);
	}
	vert_amount = b.isquad ? 4 : 3;
	for (int i = 0; i < vert_amount; i++)
	{
		//vec sub
		b.verts[i].pos.x -= level->pos.x;
		b.verts[i].pos.y -= level->pos.y;
		b.verts[i].pos.z -= level->pos.z;
		if (a_far > vec_length(b.verts[i].pos))
			return (0);
	}
	return (1);
}

int		occlusion_culling(t_tri tri, t_level *level)
{
	for (int i = 0; i < level->all.tri_amount; i++)
		if (is_bface_in_aface(level->all.tris[i], tri, level))
			if (is_aface_closer(level->all.tris[i], tri, level))
				return (0);
	return (1);
} 
