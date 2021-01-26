/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 17:50:56 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/21 20:26:12 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static int		fov_culling(t_ray c[3], t_tri tri)
{
	float	end[3];
	int		side;


	vec_sub(end, tri.verts[0].pos, c[0].pos);
	if (vec_dot(end, c[2].dir) <= 0)
	{
		vec_sub(end, tri.verts[1].pos, c[0].pos);
		if (vec_dot(end, c[2].dir) <= 0)
		{
			vec_sub(end, tri.verts[2].pos, c[0].pos);
			if (vec_dot(end, c[2].dir) <= 0)
			{
				if (tri.isquad)
				{
					vec_sub(end, tri.verts[3].pos, c[0].pos);
					if (vec_dot(end, c[2].dir) <= 0)
						return (0);
				}
				else
					return (0);
			}
		}
	}
	vec_sub(end, tri.verts[0].pos, c[0].pos);
	if (vec_dot(end, c[0].dir) <= 0)
		side = 0;
	else if (vec_dot(end, c[1].dir) <= 0)
		side = 1;
	else
		return (1);

	vec_sub(end, tri.verts[1].pos, c[0].pos);
	if (vec_dot(end, c[0].dir) <= 0)
	{
		if (side == 1)
			return (1);
		side = 0;
	}
	else if (vec_dot(end, c[1].dir) <= 0)
	{
		if (side == 0)
			return (1);
		side = 1;
	}
	else
		return (1);
	vec_sub(end, tri.verts[2].pos, c[0].pos);
	if (vec_dot(end, c[0].dir) <= 0)
	{
		if (side == 1)
			return (1);
		side = 0;
	}
	else if (vec_dot(end, c[1].dir) <= 0)
	{
		if (side == 0)
			return (1);
		side = 1;
	}
	else
		return (1);
	if (tri.isquad)
	{
		vec_sub(end, tri.verts[3].pos, c[0].pos);
		if (vec_dot(end, c[0].dir) <= 0)
		{
			if (side == 1)
				return (1);
			side = 0;
		}
		else if (vec_dot(end, c[1].dir) <= 0)
		{
			if (side == 0)
				return (1);
			side = 1;
		}
		else
			return (1);
	}
	return (0);
}

static int		distance_culling(t_tri tri, float player[3])
{
	int max = 20;
	float smallest = 9999999;
	float v[3];
	float n;
	int	v_amount;

	v_amount = tri.isquad ? 4 : 3;
	for (int i = 0; i < v_amount; i++)
	{
		vec_sub(v, tri.verts[i].pos, player);
		n = vec_length(v);
		if (n < smallest)
			smallest = n;
	}
	return ((smallest < max));
}

static int		backface_culling(t_ray r, t_tri tri)
{
	float	normal[3];
	float	diff[3];

	vec_cross(normal, tri.v0v1, tri.v0v2);
	vec_sub(diff, tri.verts[0].pos, r.pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(diff, tri.verts[1].pos, r.pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(diff, tri.verts[2].pos, r.pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	return (0);
}

t_obj		*culling(t_level *level, int *visible)
{
	float		angle = level->look_side;
	t_ray		c[3];

	c[0].pos[0] = level->pos[0];
	c[0].pos[1] = level->pos[1];
	c[0].pos[2] = level->pos[2];
	vec_rot(c[0].dir, (float[3]){0, 0, 1}, angle + ((M_PI / 2) - 0.5));
	c[1].pos[0] = level->pos[0];
	c[1].pos[1] = level->pos[1];
	c[1].pos[2] = level->pos[2];
	vec_rot(c[1].dir, (float[3]){0, 0, 1}, angle - ((M_PI / 2) - 0.5));
	c[2].pos[0] = level->pos[0];
	c[2].pos[1] = level->pos[1];
	c[2].pos[2] = level->pos[2];
	vec_rot(c[2].dir, (float[3]){0, 0, 1}, angle);

	t_obj *new = (t_obj*)malloc(sizeof(t_obj));

	for (int j = 0; j < level->obj->tri_amount; j++)
	{
		if (fov_culling(c, level->obj->tris[j]) &&
			(level->obj->distance_culling_mask[j] || distance_culling(level->obj->tris[j], level->pos)) &&
			(level->obj->backface_culling_mask[j] || backface_culling(c[2], level->obj->tris[j])))
		{
			(*visible)++;
		}
	}
	new->tris = (t_tri*)malloc(sizeof(t_tri) * (*visible));
	new->tri_amount = (*visible);
	int k = 0;
	for (int j = 0; j < level->obj->tri_amount; j++)
	{
		if (fov_culling(c, level->obj->tris[j]) &&
			(level->obj->distance_culling_mask[j] || distance_culling(level->obj->tris[j], level->pos)) &&
			(level->obj->backface_culling_mask[j] || backface_culling(c[2], level->obj->tris[j])))
		{
			new->tris[k] = level->obj->tris[j];
			k++;
		}
	}
	return (new);
}
