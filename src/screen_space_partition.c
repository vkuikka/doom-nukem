/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screen_space_partition.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 12:03:36 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/10 01:05:46 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

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

static void		calculate_corner_vectors(t_vec3 result[2], t_camera_info c, float px, int horizontal)
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

static int				left(t_tri *tri, t_level *l, int x, t_camera_info cam)
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

static int				right(t_tri *tri, t_level *l, int x, t_camera_info cam)
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

static int				over(t_tri *tri, t_level *l, int x, t_camera_info cam)
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

static int				under(t_tri *tri, t_level *l, int x, t_camera_info cam)
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

static void            find_ssp_index(t_tri *tri, t_level *level)
{
	t_camera_info	cam;
	cam.lon = -level->look_side + M_PI / 2;
	cam.lat = -level->look_up;
	rot_cam(&cam.front, cam.lon, cam.lat);
	rot_cam(&cam.up, cam.lon, cam.lat + (M_PI / 2));
	vec_cross(&cam.side, cam.up, cam.front);
	cam.pos = level->pos;
	cam.fov_y = level->ui->fov;
	cam.fov_x = level->ui->fov * ((float)RES_X / RES_Y);

	float		xmin;
	float		xmax;
	float		ymin;
	float		ymax;
	int			i;

	i = 0;
	xmax = RES_X - 1;
	ymax = RES_Y - 1;
	xmin = 0;
	ymin = 0;
	while (i < SSP_MAX_X - 1)
	{
		if (left(tri, level, (xmax + xmin) / 2, cam))
			xmax = (xmax + xmin) / 2;
		else if (right(tri, level, (xmax + xmin) / 2, cam))
			xmin = (xmax + xmin) / 2;
		else
		{
			int j = i;
			float	tmp;
			tmp = (xmax + xmin) / 2;
			while (j < SSP_MAX_X)
			{
				if (left(tri, level, (xmin + tmp) / 2, cam))
					tmp = (xmin + tmp) / 2;
				else if (right(tri, level, (xmin + tmp) / 2, cam))
					xmin = (xmin + tmp) / 2;
				else
					tmp = (xmin + tmp) / 2;
				j++;
			}
			j = i;
			tmp = (xmax + xmin) / 2;
			while (j < SSP_MAX_X)
			{
				if (right(tri, level, (xmax + tmp) / 2, cam))
					tmp = (xmax + tmp) / 2;
				else if (left(tri, level, (xmax + tmp) / 2, cam))
					xmax = (xmax + tmp) / 2;
				else
					tmp = (xmax + tmp) / 2;
				j++;
			}
			break;
		}
		i++;
	}
	i = 0;
	while (i < SSP_MAX_Y - 1)
	{
		if (under(tri, level, (ymax + ymin) / 2, cam))
			ymax = (ymax + ymin) / 2;
		else if (over(tri, level, (ymax + ymin) / 2, cam))
			ymin = (ymax + ymin) / 2;
		else
		{
			int j = i;
			float	tmp;
			tmp = (ymax + ymin) / 2;
			while (j < SSP_MAX_Y)
			{
				if (under(tri, level, (ymin + tmp) / 2, cam))
					tmp = (ymin + tmp) / 2;
				else if (over(tri, level, (ymin + tmp) / 2, cam))
					ymin = (ymin + tmp) / 2;
				else
					tmp = (ymin + tmp) / 2;
				j++;
			}
			j = i;
			tmp = (ymax + ymin) / 2;
			while (j < SSP_MAX_Y)
			{
				if (over(tri, level, (ymax + tmp) / 2, cam))
					tmp = (ymax + tmp) / 2;
				else if (under(tri, level, (ymax + tmp) / 2, cam))
					ymax = (ymax + tmp) / 2;
				else
					tmp = (ymax + tmp) / 2;
				j++;
			}
			break;
		}
		i++;
	}
	for (int x = get_ssp_coordinate(xmin, 1); x <= get_ssp_coordinate(xmax, 1); x++)
		for (int y = get_ssp_coordinate(ymin, 0); y <= get_ssp_coordinate(ymax, 0); y++)
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
	if (!(level->ssp = (t_obj*)malloc(sizeof(t_obj) * SSP_MAX_X * SSP_MAX_Y)))
		ft_error("memory allocation failed");
	for (int i = 0; i < SSP_MAX_X * SSP_MAX_Y; i++)
		if (!(level->ssp[i].tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
			ft_error("memory allocation failed");
}