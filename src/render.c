/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/30 04:08:34by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		fill_pixels(unsigned *grid, int gap)
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
				color = grid[x + (y * RES_X)];
				if ((y + 1) % gap && y + 1 < RES_Y)
					grid[x + ((y + 1) * RES_X)] = color;
			}
			else
				grid[x + (y * RES_X)] = color;
			x++;
		}
		y++;
	}
}

float		cast_face(t_tri t, t_ray ray, int *col, t_bmp *img)
{
	t_vec3	pvec;
	vec_cross(&pvec, ray.dir, t.v0v2);
	float invdet = 1 / vec_dot(pvec, t.v0v1); 

	t_vec3	tvec;
	vec_sub(&tvec, ray.pos, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;
	if (t.isgrid)
	{
		int test = 1;
		if (u < 0)
			test = -1;
		u -= (int)u / 1;
		u *= test;
	}
	if (u < 0 || u > 1)
		return 0;

	t_vec3 qvec;
	vec_cross(&qvec, tvec, t.v0v1);
	float v = vec_dot(ray.dir, qvec) * invdet;
	if (t.isgrid)
	{
		int test = 1;
		if (v < 0)
			test = -1;
		v -= (int)v / 1;
		v *= test;
	}
	if (t.isquad)
	{
		if (v < 0 || v > 1) //quad hack
			return 0;
	}
	else if (v < 0 || u + v > 1)
		return 0;
	// if (t.isgrid)
	// 	printf("%f %f\n", u, v);
    float dist = vec_dot(qvec, t.v0v2) * invdet;
	if (img && col)
	{
		*col = face_color(u, v, t, img);
		global_seginfo = "set color 2\n";
	}
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

int			fog(int color, float dist, unsigned fog_color)
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

int			skybox(t_level l, t_ray r)
{
	int		color;
	float	dist;

	color = 0;
	r.pos.x = 0;
	r.pos.y = 0;
	r.pos.z = 0;
	for (int i = 0; i < l.sky.obj.tri_amount; i++)
	{
		dist = cast_face(l.sky.obj.tris[i] , r, &color, &l.sky.img);
		if (dist > 0 && color)
			return (color);
	}
	return (color);
}


void		rot_cam(t_vec3 *cam, const float lon, const float lat)
{
	const float	phi = (M_PI / 2 - lat);
	const float	theta = lon;
	float radius = 1;

	cam->x = radius * sin(phi) * cos(theta);
	cam->y = radius * cos(phi);
	cam->z = radius * sin(phi) * sin(theta);
}

int			render(void *data_pointer)
{
	t_rthread	*t = data_pointer;
	t_ray		r;
	float		angle = t->level->look_side;
	int			pixel_gap = t->level->quality;
	int			rand_amount = 0;

	if (t->level->quality >= NOISE_QUALITY_LIMIT)
	{
		srand(SDL_GetTicks());
		rand_amount = 2;
	}
	r.pos.x = t->level->pos.x;
	r.pos.y = t->level->pos.y;
	r.pos.z = t->level->pos.z;

	t_vec3	cam;
	t_vec3	up;
	t_vec3	side;
	float lon = -t->level->look_side + M_PI/2;
	float lat = -t->level->look_up;
	rot_cam(&cam, lon, lat);
	rot_cam(&up, lon, lat + (M_PI / 2));
	vec_cross(&side, up, cam);

	for (int x = t->id; x < RES_X; x += THREAD_AMOUNT)
	{
		t_vec3 tmp;
		tmp.x = 1.0 / RES_X * x - 0.5;
		tmp.y = 0;
		tmp.z = 1;
		for (int y = 0; y < RES_Y; y++)
		{
#if TARGETFPS < 100
			if (!rand_amount || rand() % rand_amount)	//skip random pixel
#endif
			if (!(x % pixel_gap) && !(y % pixel_gap))
			{
				t->window->frame_buffer[x + (y * RES_X)] = t->level->fog_color;
				t->window->depth_buffer[x + (y * RES_X)] = 0;

				float ym = (1.0 / RES_Y * y - 0.5);	//multiply these to change fov
				float xm = (1.0 / RES_X * x - 0.5);

				r.dir.x = cam.x + up.x * ym + side.x * xm;
				r.dir.y = cam.y + up.y * ym + side.y * xm;
				r.dir.z = cam.z + up.z * ym + side.z * xm;

				int side = x < RES_X / 2 ? 0 : 1;
				for (int j = 0; j < t->level->obj[side].tri_amount; j++)
				{
					int color;
					float dist;
					dist = cast_face(t->level->obj[side].tris[j], r, &color, t->img);
					if (dist > 0 &&
						(dist < t->window->depth_buffer[x + (y * RES_X)] ||
								t->window->depth_buffer[x + (y * RES_X)] == 0))
					{
						t->window->depth_buffer[x + (y * RES_X)] = dist;
						if (t->level->enable_fog)
							t->window->frame_buffer[x + (y * RES_X)] = fog(color, dist, t->level->fog_color);
						else
							t->window->frame_buffer[x + (y * RES_X)] = color;
					}
				}
				if (!t->level->enable_fog && !t->window->depth_buffer[x + (y * RES_X)])
					t->window->frame_buffer[x + (y * RES_X)] = skybox(*t->level, r);
			}
		}
	}
	return (0);
}
