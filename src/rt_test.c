/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_test.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/12 01:20:06 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

float	rt_sphere(t_window *window, float ray[3])
{
	float	rsp[3];
	rsp[0] = 0;	//sphere pos - ray start pos
	rsp[1] = 0;
	rsp[2] = 5;

	float a = vec_dot(ray, ray);
	float b = 2 * vec_dot(ray, rsp);
	float c = vec_dot(rsp, rsp) - 10;

	float disc = b * b - 4 * a * c;

	float x0;
	float x1;

	if (disc < 0)
		return (0);
	else if (disc == 0)
		x0 = x1 = - 0.5 * b / a; 
    else
	{ 
        float q = (b > 0) ? 
            -0.5 * (b + sqrt(disc)) : 
            -0.5 * (b - sqrt(disc)); 
        x0 = q / a; 
        x1 = c / q; 
    } 
	if (x0 > x1)
		x1 = x0;

	if (x1 > 0)
		x1 = 0;
	x1 *= -1.0;

	if (!x1)
		SDL_SetRenderDrawColor(window->SDLrenderer, 0, 0, 0, 255);
	else
		SDL_SetRenderDrawColor(window->SDLrenderer, 255.0 / x1, 255.0 / x1, 255.0 / x1, 255);

	return (x1);
	// return ((-b - sqrt(disc)) / (2.0 * a));
}

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
	l->angle = 0;
	l->txtr = NULL;

	//load_obj("level/cube.obj", &l->obj[0]);
	//load_obj("level/monkey.obj", &l->obj[0]);
	load_obj("level/teapot_decimated.obj", &l->obj[0]);

	return (l);
}

float	rt_tri(t_window *window, t_tri t, t_ray ray, int x, int y, int *col)
{
	float	pvec[3];
	vec_cross(pvec, ray.dir, t.v0v2);
    float det = vec_dot(pvec, t.v0v1); 
	if (det > 0)
		return 0;
	float invdet = 1 / det;

	float	tvec[3];
	vec_sub(tvec, ray.pos, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;
	if (u < 0 || u > 1)
		return 0;

	float qvec[3];
	vec_cross(qvec, tvec, t.v0v1);
	float v = vec_dot(ray.dir, qvec) * invdet;
	// if (v < 0 || v > 1) //quad hack
	if (v < 0 || u + v > 1)
		return 0;
    float dist = vec_dot(qvec, t.v0v2) * invdet;

	// return (((int)(u * 255) & 0xff) << 16) + (((int)(v * 255) & 0xff) << 8) + (((int)((1-u-v) * 255) & 0xff));

	*col =	(((int)(u * 255) & 0xff) << 24) +
			(((int)(v * 255) & 0xff) << 16) +
			(((int)((1-u-v) * 255) & 0xff) << 8) + 0xff;
// (((int)((1-u-v) * 255) & 0xff) << 8) + (0xff - (0xff * (dist / 10)));	//smooth transition to limited draw distance

	return dist;
}

void	*rt_test(void *data_pointer)
{
	t_rthread	*t = data_pointer;
	t_obj		*obj;
	t_ray		r;

	obj = t->level->obj;

	r.pos[0] = t->level->pos[0];
	r.pos[1] = 0;
	r.pos[2] = t->level->pos[2];


	// ray.dir[0] = world.view->dir[0] - 1/RES_X * RES_X / 2;
	float angle = t->level->angle;
	t_window *window = t->window;
	// for (int x = 0; x < RES_X; x++)
	for (int x = t->id; x < RES_X; x += THREAD_AMOUNT)
	{
		float tmp[3];
		tmp[0] = 1 / RES_X * x - 0.5;
		tmp[1] = 0;
		tmp[2] = 1;
		for (int y = 0; y < RES_Y; y++)
		{
			window->depth_buffer[x + (y * (int)RES_X)] = 0;
			window->frame_buffer[x + (y * (int)RES_X)] = 0x00000000;
			if (!(x % PIXEL_GAP) && !(y % PIXEL_GAP))
			{
				vec_rot(r.dir, tmp, angle);
				r.dir[1] = 1 / RES_Y * y - 0.5;
				float closest = -1;
	
				for (int j = 0; j < obj[0].tri_amount; j++)
				{
					int color;
					float dist;

					dist = rt_tri(window, obj[0].tris[j], r, x, y, &color);
					if (dist > 0 &&
							(dist < window->depth_buffer[x + (y * (int)RES_X)] ||
									window->depth_buffer[x + (y * (int)RES_X)] == 0))
					{
						window->depth_buffer[x + (y * (int)RES_X)] = dist;
						window->frame_buffer[x + (y * (int)RES_X)] = color;
					}
					//else
					//	window->frame_buffer[x + (y * (int)RES_X)] = 0;
				}
			}
		}
	}
	return (NULL);
}
