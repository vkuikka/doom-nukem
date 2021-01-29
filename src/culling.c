/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 17:50:56 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/29 03:43:24 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static int		fov_culling(t_ray c[3], t_tri tri)
{
	t_vec3	end;
	int		side;


	vec_sub(&end, tri.verts[0].pos, c[0].pos);
	if (vec_dot(end, c[2].dir) <= 0)
	{
		vec_sub(&end, tri.verts[1].pos, c[0].pos);
		if (vec_dot(end, c[2].dir) <= 0)
		{
			vec_sub(&end, tri.verts[2].pos, c[0].pos);
			if (vec_dot(end, c[2].dir) <= 0)
			{
				if (tri.isquad)
				{
					vec_sub(&end, tri.verts[3].pos, c[0].pos);
					if (vec_dot(end, c[2].dir) <= 0)
						return (0);
				}
				else
					return (0);
			}
		}
	}
	vec_sub(&end, tri.verts[0].pos, c[0].pos);
	if (vec_dot(end, c[0].dir) <= 0)
		side = 0;
	else if (vec_dot(end, c[1].dir) <= 0)
		side = 1;
	else
		return (1);

	vec_sub(&end, tri.verts[1].pos, c[0].pos);
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
	vec_sub(&end, tri.verts[2].pos, c[0].pos);
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
		vec_sub(&end, tri.verts[3].pos, c[0].pos);
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

static int		distance_culling(t_tri tri, t_vec3 player)
{
	int max = 20;
	float smallest = 9999999;
	t_vec3	v;
	float n;
	int	v_amount;

	v_amount = tri.isquad ? 4 : 3;
	for (int i = 0; i < v_amount; i++)
	{
		vec_sub(&v, tri.verts[i].pos, player);
		n = vec_length(v);
		if (n < smallest)
			smallest = n;
	}
	return ((smallest < max));
}

static int		backface_culling(t_ray r, t_tri tri)
{
	t_vec3	normal;
	t_vec3	diff;

	vec_cross(&normal, tri.v0v1, tri.v0v2);
	vec_sub(&diff, tri.verts[0].pos, r.pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(&diff, tri.verts[1].pos, r.pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(&diff, tri.verts[2].pos, r.pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	return (0);
}

void			culling(t_level *level, int *visible, t_obj *culled)
{
	float		angle = level->look_side;
	t_ray		c[3];

	c[0].pos.x = level->pos.x;
	c[0].pos.y = level->pos.y;
	c[0].pos.z = level->pos.z;
	vec_rot(&c[0].dir, (t_vec3){0, 0, 1}, angle + ((M_PI / 2) - 0.5));
	c[1].pos.x = level->pos.x;
	c[1].pos.y = level->pos.y;
	c[1].pos.z = level->pos.z;
	vec_rot(&c[1].dir, (t_vec3){0, 0, 1}, angle - ((M_PI / 2) - 0.5));
	c[2].pos.x = level->pos.x;
	c[2].pos.y = level->pos.y;
	c[2].pos.z = level->pos.z;
	vec_rot(&c[2].dir, (t_vec3){0, 0, 1}, angle);

	int k = 0;
	for (int j = 0; j < level->obj->tri_amount; j++)
	{
		if (fov_culling(c, level->obj->tris[j]) &&
			(level->obj->distance_culling_mask[j] || distance_culling(level->obj->tris[j], level->pos)) &&
			(level->obj->backface_culling_mask[j] || backface_culling(c[2], level->obj->tris[j])))
		{
			culled[0].tris[k] = level->obj->tris[j];
			k++;
		}
	}
	(*visible) = k;
	culled[0].tri_amount = k;
}
