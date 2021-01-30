/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/30 02:23:27 by vkuikka          ###   ########.fr       */
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
	while (y < RES_Y)
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

float	cast_face(t_tri t, t_ray ray, int *col, t_bmp *img)
{
	t_vec3	pvec;
	vec_cross(&pvec, ray.dir, t.v0v2);
	float invdet = 1 / vec_dot(pvec, t.v0v1); 

	t_vec3	tvec;
	vec_sub(&tvec, ray.pos, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;
	if (u < 0 || u > 1)
		return 0;

	t_vec3 qvec;
	vec_cross(&qvec, tvec, t.v0v1);
	float v = vec_dot(ray.dir, qvec) * invdet;
	if (t.isquad)
	{
		if (v < 0 || v > 1) //quad hack
			return 0;
	}
	else if (v < 0 || u + v > 1)
		return 0;
    float dist = vec_dot(qvec, t.v0v2) * invdet;
	if (img)
		*col = face_color(u, v, t, img);
	return dist;
}

unsigned	crossfade(unsigned color1, unsigned color2, unsigned fade, unsigned r1)
{
	unsigned g1;
	unsigned b1;
	unsigned r2;
	unsigned g2;
	unsigned b2;

	r1 = ((color1 % (0x1000000)) >> 8 * 2);
	g1 = ((color1 % (0x10000)) >> 8 * 1);
	b1 = (color1 % (0x100));
	r2 = ((color2 % (0x1000000)) >> 8 * 2);
	g2 = ((color2 % (0x10000)) >> 8 * 1);
	b2 = (color2 % (0x100));

	unsigned newr = (r1 * (0xff - fade) + r2 * fade) / 0xff;
	unsigned newg = (g1 * (0xff - fade) + g2 * fade) / 0xff;
	unsigned newb = (b1 * (0xff - fade) + b2 * fade) / 0xff;
	return ((newr << 8 * 3) + (newg << 8 * 2) + (newb << 8 * 1) + 0xff);
}

int		fog(int color, float dist, unsigned fog_color)
{
	float	fade;

	if (dist < 20)
	{
		fade = (dist + 1) / 20;
		fade = fade > 1 ? 1 : fade;
		return (crossfade(color >> 8, fog_color >> 8, 0xff * fade, 0));
	}
	return (fog_color);
}

int		render(void *data_pointer)
{
	t_rthread	*t = data_pointer;
	t_ray		r;
	float		angle = t->level->look_side;
	int			pixel_gap = t->level->quality;
	int			rand_amount = 10000000;

	if (t->level->quality > 7)
		rand_amount = 2;
	r.pos.x = t->level->pos.x;
	r.pos.y = t->level->pos.y;
	r.pos.z = t->level->pos.z;

	// for (int x = 0; x < RES_X; x++)
	for (int x = t->id; x < RES_X; x += THREAD_AMOUNT)
	{
		t_vec3 tmp;
		tmp.x = 1 / RES_X * x - 0.5;
		tmp.y = 0;
		tmp.z = 1;
		for (int y = 0; y < RES_Y; y++)
		{
			if (rand() % rand_amount)	//skip random pixel
			if (!(x % pixel_gap) && !(y % pixel_gap))
			{
				t->window->frame_buffer[x + (y * (int)RES_X)] = t->level->fog_color;
				t->window->depth_buffer[x + (y * (int)RES_X)] = 0;
				vec_rot(&r.dir, tmp, angle);

				r.dir.y = (1 / RES_Y * y) - t->level->look_up;
				// vec_normalize(r.dir);

				int side = x < RES_X / 2 ? 0 : 1;
				for (int j = 0; j < t->level->obj[side].tri_amount; j++)
				{
					int color;
					float dist;
					dist = cast_face(t->level->obj[side].tris[j], r, &color, t->img);
					if (dist > 0 &&
						(dist < t->window->depth_buffer[x + (y * (int)RES_X)] ||
								t->window->depth_buffer[x + (y * (int)RES_X)] == 0))
					{
						t->window->depth_buffer[x + (y * (int)RES_X)] = dist;
						if (t->level->enable_fog)
							t->window->frame_buffer[x + (y * (int)RES_X)] = fog(color, dist, t->level->fog_color);
						else
							t->window->frame_buffer[x + (y * (int)RES_X)] = color;
					}
				}
			}
		}
	}
	return (0);
}
