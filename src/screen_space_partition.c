/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screen_space_partition.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 12:03:36 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/17 23:58:35 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

int			get_ssp_index(int xd, int yd)
{
	return ((int)((float)xd / RES_X * SSP_MAX_X) + (int)((float)yd / RES_Y * SSP_MAX_Y) * SSP_MAX_X);
}

int			get_ssp_coordinate(int coord, int horizontal)
{
	if (horizontal)
		return ((int)((float)coord / RES_X * SSP_MAX_X));
	return ((int)((float)coord / RES_Y * SSP_MAX_Y));
}

static void		calculate_corner_vectors(t_vec3 result[2], t_camera c, float px, int horizontal)
{
	float	ym;
	float	xm;

	if (horizontal)
	{
		ym = c.fov_y / RES_Y * RES_Y - c.fov_y / 2;
		xm = c.fov_x / RES_X * px - c.fov_x / 2;
	}
	else
	{
		ym = c.fov_y / RES_Y * px - c.fov_y / 2;
		xm = c.fov_x / RES_X * RES_X - c.fov_x / 2;
	}
	result[0].x = c.front.x + c.up.x * ym + c.side.x * xm;
	result[0].y = c.front.y + c.up.y * ym + c.side.y * xm;
	result[0].z = c.front.z + c.up.z * ym + c.side.z * xm;
	if (horizontal)
	{
		ym = c.fov_y / RES_Y * 0 - c.fov_y / 2;
		xm = c.fov_x / RES_X * px - c.fov_x / 2;
	}
	else
	{
		ym = c.fov_y / RES_Y * px - c.fov_y / 2;
		xm = c.fov_x / RES_X * 0 - c.fov_x / 2;
	}
	result[1].x = c.front.x + c.up.x * ym + c.side.x * xm;
	result[1].y = c.front.y + c.up.y * ym + c.side.y * xm;
	result[1].z = c.front.z + c.up.z * ym + c.side.z * xm;
}

static int				left(t_tri *tri, int x, t_camera cam)
{
	t_vec3	corners[2];
	t_vec3	normal;
	t_vec3	vert;

	calculate_corner_vectors(corners, cam, (float)x, 1);
	vec_cross(&normal, corners[0], corners[1]);
	for (int i = 0; i < 3 + tri->isquad; i++)
	{
		vec_sub(&vert, tri->verts[i].pos, cam.pos);
		if (vec_dot(vert, normal) > 0)
			return (0);
	}
	return (1);
}

static int				right(t_tri *tri, int x, t_camera cam)
{
	t_vec3	corners[2];
	t_vec3	normal;
	t_vec3	vert;

	calculate_corner_vectors(corners, cam, (float)x, 1);
	vec_cross(&normal, corners[1], corners[0]);
	for (int i = 0; i < 3 + tri->isquad; i++)
	{
		vec_sub(&vert, tri->verts[i].pos, cam.pos);
		if (vec_dot(vert, normal) > 0)
			return (0);
	}
	return (1);
}

static int				over(t_tri *tri, int x, t_camera cam)
{
	t_vec3	corners[2];
	t_vec3	normal;
	t_vec3	vert;

	calculate_corner_vectors(corners, cam, (float)x, 0);
	vec_cross(&normal, corners[0], corners[1]);
	for (int i = 0; i < 3 + tri->isquad; i++)
	{
		vec_sub(&vert, tri->verts[i].pos, cam.pos);
		if (vec_dot(vert, normal) > 0)
			return (0);
	}
	return (1);
}

static int				under(t_tri *tri, int x, t_camera cam)
{
	t_vec3	corners[2];
	t_vec3	normal;
	t_vec3	vert;

	calculate_corner_vectors(corners, cam, (float)x, 0);
	vec_cross(&normal, corners[1], corners[0]);
	for (int i = 0; i < 3 + tri->isquad; i++)
	{
		vec_sub(&vert, tri->verts[i].pos, cam.pos);
		if (vec_dot(vert, normal) > 0)
			return (0);
	}
	return (1);
}

static void				find_partition(int (*dir1)(t_tri*, int, t_camera),
										int (*dir2)(t_tri*, int, t_camera), t_tri *tri, t_camera cam, float bounds[3])
{
	float	min = bounds[0];
	float	max = bounds[1];
	int		i;

	i = 0;
	while (i < bounds[2])
	{
		if (dir1(tri, (max + min) / 2, cam))
			max = (max + min) / 2;
		else if (dir2(tri, (max + min) / 2, cam))
			min = (max + min) / 2;
		else
		{
			int j = i;
			float	tmp;
			tmp = (max + min) / 2;
			while (j < bounds[2])
			{
				if (dir1(tri, (min + tmp) / 2, cam))
					tmp = (max + tmp) / 2;
				else if (dir2(tri, (min + tmp) / 2, cam))
					min = (min + tmp) / 2;
				else
					tmp = (min + tmp) / 2;
				j++;
			}
			j = i;
			tmp = (max + min) / 2;
			while (j < bounds[2])
			{
				if (dir2(tri, (max + tmp) / 2, cam))
					tmp = (max + tmp) / 2;
				else if (dir1(tri, (max + tmp) / 2, cam))
					max = (max + tmp) / 2;
				else
					tmp = (max + tmp) / 2;
				j++;
			}
			break;
		}
		i++;
	}
	bounds[0] = min;
	bounds[1] = max;
}

static void            find_ssp_index(t_tri *tri, t_level *level)
{
	t_camera	cam = level->cam;
	float		y_bounds[3];
	float		x_bounds[3];

	x_bounds[0] = 0;
	x_bounds[1] = RES_X - 1;
	x_bounds[2] = SSP_MAX_X - 1;
	y_bounds[0] = 0;
	y_bounds[1] = RES_Y - 1;
	y_bounds[2] = SSP_MAX_Y - 1;
	find_partition(left, right, tri, cam, x_bounds);
	find_partition(under, over, tri, cam, y_bounds);
	for (int x = get_ssp_coordinate(x_bounds[0], 1); x <= get_ssp_coordinate(x_bounds[1], 1); x++)
		for (int y = get_ssp_coordinate(y_bounds[0], 0); y <= get_ssp_coordinate(y_bounds[1], 0); y++)
			level->ssp[x + y * SSP_MAX_X].tris[level->ssp[x + y * SSP_MAX_X].tri_amount++] = *tri;
}

void		screen_space_partition(t_level *level)
{
	for (int i = 0; i < SSP_MAX_X * SSP_MAX_Y; i++)
		level->ssp[i].tri_amount =  0;
	for (int i = 0; i < level->visible.tri_amount; i++)
	{
		if (level->visible.tris[i].isgrid)
		{
			for (int o = 0; o < SSP_MAX_X * SSP_MAX_Y; o++)
				level->ssp[o].tris[level->ssp[o].tri_amount++] = level->visible.tris[i];
		}
		else
			find_ssp_index(&level->visible.tris[i], level);
	}
}

void		init_screen_space_partition(t_level *level)
{
	if (level->ssp)
	{
		for (int i = 0; i < SSP_MAX_X * SSP_MAX_Y; i++)
			free(level->ssp[i].tris);
		free(level->ssp);
	}
	if (!(level->ssp = (t_obj*)malloc(sizeof(t_obj) * SSP_MAX_X * SSP_MAX_Y)))
		ft_error("memory allocation failed");
	for (int i = 0; i < SSP_MAX_X * SSP_MAX_Y; i++)
		if (!(level->ssp[i].tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
			ft_error("memory allocation failed");
}