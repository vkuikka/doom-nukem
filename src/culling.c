/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 17:50:56 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/08 17:57:06 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static int		cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri)
{
	t_vec3	vert;
	int		i;

	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&vert, tri.verts[i].pos, pos);
		if (vec_dot(dir, vert) > 0)
			return (1);
		i++;
	}
	return (0);
}

static int		fov_culling(t_vec3 side_normal[4], t_vec3 pos, t_tri tri)
{
	t_vec3	end;
	int		out[4][4];
	int		i;

	out[3][0] = 1;
	out[3][1] = 1;
	out[3][2] = 1;
	out[3][3] = 1;
	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&end, tri.verts[i].pos, pos);
		out[i][0] = vec_dot(side_normal[0], end) < 0;
		out[i][1] = vec_dot(side_normal[1], end) < 0;
		out[i][2] = vec_dot(side_normal[2], end) < 0;
		out[i][3] = vec_dot(side_normal[3], end) < 0;
		if (!out[i][0] && !out[i][1] && !out[i][2] && !out[i][3])
			return (1);
		i++;
	}
	i = 0;
	while (i < 4)
	{
		if (out[0][i] && out[1][i] && out[2][i] && out[3][i])
			return (0);
		i++;
	}
	return (1);
}

static int		distance_culling(t_tri tri, t_vec3 player, float render_distance)
{
	t_vec3	v;
	float	min = FLT_MAX;
	float	max = 0;
	float	len;

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

static int		backface_culling(t_vec3 pos, t_tri tri)
{
	t_vec3	normal;
	t_vec3	diff;

	vec_cross(&normal, tri.v0v1, tri.v0v2);
	vec_sub(&diff, tri.verts[0].pos, pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(&diff, tri.verts[1].pos, pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(&diff, tri.verts[2].pos, pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	return (0);
}

int			normal_plane_culling(t_tri tri, t_vec3 *pos, t_vec3 *dir)
{
	t_vec3	test;

	for (int i = 0; i < 3 + tri.isquad; i++)
	{
		vec_sub(&test, tri.verts[i].pos, *pos);
		if (vec_dot(test, *dir) <= 0)
			return (FALSE);
	}
	return (TRUE);
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

void			calculate_side_normals(t_vec3 normal[4], t_vec3 corner[4])
{
	vec_cross(&normal[0], corner[2], corner[0]);	//left
	vec_cross(&normal[1], corner[1], corner[3]);	//right
	vec_cross(&normal[2], corner[0], corner[1]);	//top
	vec_cross(&normal[3], corner[3], corner[2]);	//bot
}

void			calculate_corner_vectors(t_vec3 corner[4], t_level *level, t_vec3 front)
{
	t_vec3	up;
	t_vec3	side;
	float	lon = -level->look_side + M_PI / 2;
	float	lat = -level->look_up;
	float	fov_x = level->ui->fov * ((float)RES_X / RES_Y);
	rot_cam(&front, lon, lat);
	rot_cam(&up, lon, lat + (M_PI / 2));
	vec_cross(&side, up, front);	

	float ym = -level->ui->fov / 2;
	float xm = -fov_x / 2;
	corner[0].x = front.x + up.x * ym + side.x * xm;
	corner[0].y = front.y + up.y * ym + side.y * xm;
	corner[0].z = front.z + up.z * ym + side.z * xm;

	ym = -level->ui->fov / 2;
	xm = fov_x - fov_x / 2;
	corner[1].x = front.x + up.x * ym + side.x * xm;
	corner[1].y = front.y + up.y * ym + side.y * xm;
	corner[1].z = front.z + up.z * ym + side.z * xm;

	ym = level->ui->fov - level->ui->fov / 2;
	xm = -fov_x / 2;
	corner[2].x = front.x + up.x * ym + side.x * xm;
	corner[2].y = front.y + up.y * ym + side.y * xm;
	corner[2].z = front.z + up.z * ym + side.z * xm;

	ym = level->ui->fov - level->ui->fov / 2;
	xm = fov_x - fov_x / 2;
	corner[3].x = front.x + up.x * ym + side.x * xm;
	corner[3].y = front.y + up.y * ym + side.y * xm;
	corner[3].z = front.z + up.z * ym + side.z * xm;
}

void			culling(t_level *level)
{
	t_vec3		front;
	t_vec3		corner[4];
	t_vec3		side_normals[4];

	vec_rot(&front, (t_vec3){0, 0, 1}, level->look_side);
	calculate_corner_vectors(corner, level, front);
	calculate_side_normals(side_normals, corner);
	int visible_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].isgrid ||
			(cull_behind(front, level->pos, level->all.tris[i]) && fov_culling(side_normals, level->pos, level->all.tris[i]) &&
			(!level->ui->distance_culling || level->all.distance_culling_mask[i] || distance_culling(level->all.tris[i], level->pos, level->ui->render_distance)) &&
			(!level->ui->backface_culling || level->all.backface_culling_mask[i] || backface_culling(level->pos, level->all.tris[i]))))
		{
			level->visible.tris[visible_amount] = level->all.tris[i];
			visible_amount++;
		}
	}
	level->visible.tri_amount = visible_amount;
	if (level->ui->backface_culling)
	{
		visible_amount = 0;
		for (int i = 0; i < level->visible.tri_amount; i++)
		{
			if (occlusion_culling(level->visible.tris[i], level))
			{
				level->visible.tris[visible_amount] = level->visible.tris[i];
				visible_amount++;
			}
		}
	}
	level->visible.tri_amount = visible_amount;
	reflection_culling(level);
}
