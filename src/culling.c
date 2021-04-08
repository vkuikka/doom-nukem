/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 17:50:56 by vkuikka           #+#    #+#             */
/*   Updated: 2021/03/26 03:21:01 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static int		fov_culling(t_ray c[3], t_tri tri)
{
	t_vec3	end;
	int		side;

	global_seginfo = "culling fov\n";

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

static int		distance_culling(t_tri tri, t_vec3 player, float render_distance)
{
	t_vec3	v;
	float	min = FLT_MAX;
	float	max = 0;
	float	len;

	global_seginfo = "culling distance\n";
	for (int i = 0; i < 3 + tri.isquad; i++)
	{
		vec_sub(&v, tri.verts[i].pos, player);
		len = vec_length(v);
		if (len < min)
			min = len;
		if (len > max)
			max = len;
	}
	if (min < render_distance)
		return (1);
	t_vec3	v1v2;
	vec_sub(&v1v2, tri.verts[2].pos, tri.verts[1].pos);
	min = vec_length(v1v2);
	len = vec_length(tri.v0v1);
	if (len < min)
		len = min;
	min = vec_length(tri.v0v2);
	if (len < min)
		len = min;
	if (tri.isquad)
	{
		t_vec3	v0v3;
		t_vec3	v1v3;
		t_vec3	v2v3;
		vec_sub(&v0v3, tri.verts[3].pos, tri.verts[0].pos);
		vec_sub(&v1v3, tri.verts[3].pos, tri.verts[1].pos);
		vec_sub(&v2v3, tri.verts[3].pos, tri.verts[2].pos);
		min = vec_length(v0v3);
		if (len < min)
			len = min;
		min = vec_length(v1v2);
		if (len < min)
			len = min;
		min = vec_length(v1v3);
		if (len < min)
			len = min;
	}
	return (max < len);
}

static int		backface_culling(t_ray r, t_tri tri)
{
	t_vec3	normal;
	t_vec3	diff;

	global_seginfo = "culling backface\n";
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

void		reflection_culling(t_level *level)
{
	for (int i = 0; i < level->visible.tri_amount; i++)
	{
		if (level->visible.tris[i].reflectivity)
		{
			//ft_bzero(mask);
			for (int k = 0; k < level->all.tri_amount; k++)
			{
				// if (normal_plane_culling() && backface_culling(normal), distance?, occlusion)
					// level->visible->tris[i]->reflection_culling_mask[k] = 1;
			}
		}
	}
}

void			culling(t_level *level)
{
	float		angle = level->look_side;
	t_ray		c[3];

	global_seginfo = "culling start\n";
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

	int visible_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].isgrid || (fov_culling(c, level->all.tris[i]) &&
			(!level->ui->distance_culling || level->all.distance_culling_mask[i] || distance_culling(level->all.tris[i], level->pos, level->ui->render_distance)) &&
			(!level->ui->backface_culling || level->all.backface_culling_mask[i] || backface_culling(c[2], level->all.tris[i])) &&
			occlusion_culling(level->all.tris[i], level)))
		{
			level->visible.tris[visible_amount] = level->all.tris[i];
			visible_amount++;
		}
	}
	level->visible.tri_amount = visible_amount;
	reflection_culling(level);
	global_seginfo = "culling end\n";
}
