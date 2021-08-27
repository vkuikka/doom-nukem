/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raster.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 23:51:41 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/27 15:37:09by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

t_ray	ray_set(t_camera *cam, float fov, t_vec2 xy)
{
	t_ray	res;
	float	xm;
	float	ym;

	xm = fov * ((float)RES_X / RES_Y);
	ym = fov;
	xm = xm / RES_X * (int)xy.x - xm / 2;
	ym = ym / RES_Y * (int)xy.y - ym / 2;
	res.dir.x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	res.dir.y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	res.dir.z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
	res.pos = cam->pos;
	return (res);
}

void			ss_minmax(t_vec2 *min, t_vec2 *max, t_ss_tri tri, int resx, int resy)
{
	min->x = tri.verts[0].pos.x;
	min->x = fmin(tri.verts[1].pos.x, min->x);
	min->x = fmin(tri.verts[2].pos.x, min->x);

	min->y = tri.verts[0].pos.y;
	min->y = fmin(tri.verts[1].pos.y, min->y);
	min->y = fmin(tri.verts[2].pos.y, min->y);

	max->x = tri.verts[0].pos.x;
	max->x = fmax(tri.verts[1].pos.x, max->x);
	max->x = fmax(tri.verts[2].pos.x, max->x);

	max->y = tri.verts[0].pos.y;
	max->y = fmax(tri.verts[1].pos.y, max->y);
	max->y = fmax(tri.verts[2].pos.y, max->y);

	min->x = fmax(0, min->x);
	min->y = fmax(0, min->y);

	max->x = fmin(resx, max->x);
	max->y = fmin(resy, max->y);
}

t_vec3			ss_to_uv(t_vec2 fp, t_ss_tri *tri, t_bmp *txtr, float *dist)
{
	t_vec2	v0;
	t_vec2	v1;
	t_vec2	v2;
	t_vec3	uvw;

	v0 = *(t_vec2*)&tri->verts[0].pos;
	v1 = *(t_vec2*)&tri->verts[1].pos;
	v2 = *(t_vec2*)&tri->verts[2].pos;
	uvw.z = fabs((fp.x * (v1.y - v2.y) +
				v1.x * (v2.y - fp.y) +
				v2.x * (fp.y - v1.y)));
	uvw.y = fabs((v0.x * (fp.y - v2.y) +
				fp.x * (v2.y - v0.y) +
				v2.x * (v0.y - fp.y)));
	uvw.x = fabs((v0.x * (v1.y - fp.y) +
				v1.x * (fp.y - v0.y) +
				fp.x * (v0.y - v1.y)));
	uvw.z *= 1 / tri->verts[0].pos.z;
	uvw.y *= 1 / tri->verts[1].pos.z;
	uvw.x *= 1 / tri->verts[2].pos.z;

	*dist = (uvw.z * tri->verts[0].pos.z +
			uvw.y * tri->verts[1].pos.z +
			uvw.x * tri->verts[2].pos.z) / (float)(uvw.x + uvw.y + uvw.z);

	normalize_data(&uvw);
	return (uvw);
}

t_ivec2	raster_texture_coord(float u, float v, t_ss_tri t, t_bmp *texture)
{
	t_ivec2	res;
	float	w;

	w = 1 - u - v;
	res.x =	((t.verts[0].uv.x * texture->width * w +
			t.verts[1].uv.x * texture->width * v +
			t.verts[2].uv.x * texture->width * u) / (float)(u + v + w));
	res.y =	((t.verts[0].uv.y * texture->height * w +
			t.verts[1].uv.y * texture->height * v +
			t.verts[2].uv.y * texture->height * u) / (float)(u + v + w));
	wrap_coords(&res.x, &res.y, texture->width, texture->height);
	return(res);
}

int				raster(void *data_pointer)
{
	t_rthread		*t = data_pointer;
	t_cast_result	res;
	t_vec2			screen;
	t_vec3			uv;
	t_vec2			min;
	t_vec2			max;
	t_ss_tri		*all;
	t_ss_tri		*tri;
	int				i;

	int resx = RES_X / t->level->ui.raycast_quality;
	int resy = RES_Y / t->level->ui.raycast_quality;

	i = 0;
	res.raytracing = FALSE;
	all = &t->level->ss_tris[0];
	while (i < t->level->ss_tri_amount)
	{
		tri = &all[i];
		ss_minmax(&min, &max, *tri, resx, resy);
		if (min.x < resx / THREAD_AMOUNT * (t->id + 0))
			min.x = resx / THREAD_AMOUNT * (t->id + 0);
		if (max.x > resx / THREAD_AMOUNT * (t->id + 1))
			max.x = resx / THREAD_AMOUNT * (t->id + 1);
		screen.x = min.x;
		while (screen.x < max.x)
		{
			screen.y = min.y;
			while (screen.y < max.y)
			{
				t_vec2 a;
				t_vec2 b;
				t_vec2 c;
				a.x = tri->verts[0].pos.x;
				a.y = tri->verts[0].pos.y;
				b.x = tri->verts[1].pos.x;
				b.y = tri->verts[1].pos.y;
				c.x = tri->verts[2].pos.x;
				c.y = tri->verts[2].pos.y;
				if (!t->window->frame_buffer[(int)screen.x + ((int)screen.y * RES_X)]
				&& point_in_tri(screen, a, b, c))
				{
					float dist;
					uv = ss_to_uv(screen, tri, &t->level->texture, &dist);
					t_ivec2	txtr = raster_texture_coord(uv.x, uv.y, *tri, &t->level->texture);

					res.ray = ray_set(&t->level->cam, t->level->ui.fov, screen);
					vec_mult(&res.ray.dir, dist);
					vec_add(&res.ray.pos, res.ray.pos, res.ray.dir);

					res.color = t->level->texture.image[txtr.x + txtr.y * t->level->texture.width];
					res.normal = get_normal(t->level->normal_map.image[txtr.x + txtr.y * t->level->normal_map.width]);
					vec_normalize(&res.normal);

					t_color light;
					light = lights(t->level, &res, res.normal);
					res.color = brightness(res.color >> 8, light) + (res.color << 24 >> 24);
					t->window->frame_buffer[(int)screen.x + ((int)screen.y * RES_X)] = res.color;
				}
				screen.y++;
			}
			screen.x++;
		}
		i++;
	}
	return (0);
}
