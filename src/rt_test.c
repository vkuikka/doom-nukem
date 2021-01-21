/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_test.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/15 02:17:142 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void			fill_pixels(unsigned *grid, int gap)
{
	int		color;
	int		i;
	int		x;
	int		y;

	y = 0;
	while (y < RES_Y - gap)
	{
		x = 0;
		color = 0;
		while (x < RES_X)
		{
			if (!(x % gap))
			{
				color = grid[x + (y * (int)RES_X)];
				if ((y + 1) % gap)
					grid[x + ((y + 1) * (int)RES_X)] = color;
			}
			else
				grid[x + (y * (int)RES_X)] = color;
			x++;
		}
		y++;
	}
}

t_level			*rt_test_init_level()
{
	t_level		*l;

	if (!(l = (t_level *)malloc(sizeof(t_level))) ||
		!(l->obj = (t_obj *)malloc(sizeof(t_obj) * 1)))
		ft_error("memory allocation failed\n");

	l->pos[0] = 0;
	l->pos[1] = 0;
	l->pos[2] = 0;
	l->look_side = 0;
	l->look_up = 0.5;
	l->txtr = NULL;

	load_obj("level/two.obj", &l->obj[0]);
	//load_obj("level/test.obj", &l->obj[0]);
	//load_obj("level/cube.obj", &l->obj[0]);
	// load_obj("level/island.obj", &l->obj[0]);
	// load_obj("level/cache.obj", &l->obj[0]);
	// load_obj("level/ship.obj", &l->obj[0]);
	// load_obj("level/one_tri.obj", &l->obj[0]);
	// load_obj("level/torus.obj", &l->obj[0]);
	// load_obj("level/monkey.obj", &l->obj[0]);
	// load_obj("level/teapot_decimated.obj", &l->obj[0]);

	return (l);
}

float	rt_tri(t_tri t, t_ray ray, int *col)
{
	float	pvec[3];
	vec_cross(pvec, ray.dir, t.v0v2);
	float invdet = 1 / vec_dot(pvec, t.v0v1); 

	float	tvec[3];
	vec_sub(tvec, ray.pos, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;
	if (u < 0 || u > 1)
		return 0;

	float qvec[3];
	vec_cross(qvec, tvec, t.v0v1);
	float v = vec_dot(ray.dir, qvec) * invdet;
	if (t.isquad)
	{
		if (v < 0 || v > 1) //quad hack
			return 0;
	}
	else if (v < 0 || u + v > 1)
		return 0;
    float dist = vec_dot(qvec, t.v0v2) * invdet;

	*col =	(((int)(u * 255) & 0xff) << 24) +
			(((int)(v * 255) & 0xff) << 16) +
			(((int)((1-u-v) * 255) & 0xff) << 8) + 0xff;
// (((int)((1-u-v) * 255) & 0xff) << 8) + (0xff - (0xff * (dist / 10)));	//smooth transition to limited draw distance
	return dist;
}

void	*rt_test(void *data_pointer)
{
	t_rthread	*t = data_pointer;
	t_ray		r;
	float		angle = t->level->look_side;

	r.pos[0] = t->level->pos[0];
	r.pos[1] = t->level->pos[1];
	r.pos[2] = t->level->pos[2];

	// for (int x = 0; x < RES_X; x++)
	for (int x = t->id; x < RES_X; x += THREAD_AMOUNT)
	{
		float tmp[3];
		tmp[0] = 1 / RES_X * x - 0.5;
		tmp[1] = 0;
		tmp[2] = 1;
		for (int y = 0; y < RES_Y; y++)
		{
			// if (rand() % 5)	//skip random pixel
			if (!(x % PIXEL_GAP) && !(y % PIXEL_GAP))
			{
				t->window->frame_buffer[x + (y * (int)RES_X)] = 0x00000000;
				t->window->depth_buffer[x + (y * (int)RES_X)] = 0;
				vec_rot(r.dir, tmp, angle);

				r.dir[1] = (1 / RES_Y * y) - t->level->look_up;
				// vec_normalize(r.dir);

				float closest = -1;
				for (int j = 0; j < t->level->obj[0].tri_amount; j++)
				{
					int color;
					float dist;
					dist = rt_tri(t->level->obj[0].tris[j], r, &color);
					if (dist > 0 &&
						(dist < t->window->depth_buffer[x + (y * (int)RES_X)] ||
								t->window->depth_buffer[x + (y * (int)RES_X)] == 0))
					{
						t->window->depth_buffer[x + (y * (int)RES_X)] = dist;
						t->window->frame_buffer[x + (y * (int)RES_X)] = color;
					}
				}
			}
		}
	}
	return (NULL);
}
