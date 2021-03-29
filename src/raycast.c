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
    float dist = vec_dot(qvec, t.v0v2) * invdet;
	if (img && col)
		*col = face_color(u, v, t, img);
	return dist;
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

void		cast_transparent(t_ray r, t_obj *obj, t_bmp *texture, t_cast_result *res)
{
	float	tmp_dist;
	int		tmp_color;
	int		transparent_face;
	int		i;

	i = 0;
	res->transparent_dist = 0;
	while (i < obj->tri_amount)
	{
		if (obj->tris[i].opacity &&
			0 < (tmp_dist = cast_face(obj->tris[i], r, &tmp_color, texture)) &&
			tmp_dist < res->dist && tmp_dist > res->transparent_dist)
		{
			transparent_face = i;
			res->transparent_dist = tmp_dist;
			res->transparent_color = tmp_color;
		}
		if (++i == obj->tri_amount && res->transparent_dist && !(i = 0))
		{
			res->dist = res->transparent_dist;
			*res->color = crossfade((unsigned)*res->color >> 8,
res->transparent_color >> 8, obj->tris[transparent_face].opacity * 0xff, 0);
			res->transparent_dist = 0;
		}
	}
}

float		rt_shadow(t_ray r, t_rthread *t, t_tri hit)
{
	t_vec3			normal;
	int				direct_shadow;
	int				i;

	vec_add(&r.pos, r.dir, r.pos);
	r.dir.x = t->level->sun_dir.x;
	r.dir.y = t->level->sun_dir.y;
	r.dir.z = t->level->sun_dir.z;
	direct_shadow = 0;
	i = 0;
	while (i < t->level->all.tri_amount && !direct_shadow)
	{
		if (0 > cast_face(t->level->all.tris[i], r, NULL, NULL))
			direct_shadow = 1;
		i++;
	}
	if (!direct_shadow)
	{
		vec_cross(&normal, hit.v0v1, hit.v0v2);
		vec_normalize(&normal);
		return ((1 - vec_dot(normal, t->level->sun_dir)) * t->level->sun_contrast);
	}
	return (t->level->direct_shadow_contrast);
}

int			cast_reflection(t_ray r, t_rthread *t, t_tri hit)
{
	float			dist;
	float			tmp_dist;
	int				tmp_color;
	int				res_col;
	int				i;

	t_ray	normal;
	vec_add(&normal.pos, r.dir, r.pos);
	vec_normalize(&r.dir);
	vec_cross(&normal.dir, hit.v0v1, hit.v0v2);
	vec_normalize(&normal.dir);
	vec_mult(&normal.dir, vec_dot(r.dir, normal.dir) * -2);
	vec_add(&r.dir, r.dir, normal.dir);
	vec_normalize(&r.dir);
	r.pos.x = normal.pos.x;
	r.pos.y = normal.pos.y;
	r.pos.z = normal.pos.z;
	dist = FLT_MAX;
	res_col = 0;
	i = 0;
	while (i < t->level->all.tri_amount)
	{
		if (0 < (tmp_dist = cast_face(t->level->all.tris[i], r, &tmp_color, t->img)) &&
			tmp_dist < dist)
		{
			res_col = tmp_color;
			dist = tmp_dist;
		}
		i++;
	}
	if (dist == FLT_MAX)
		res_col = skybox(*t->level, r);
	return (res_col);
}

float		cast_all_color(t_ray r, t_rthread *t, int side, int *color)
{
	t_cast_result	res;
	int				i;
	int				tmp_color;
	float			tmp_dist;
	int				hit;

	i = 0;
	res.dist = FLT_MAX;
	res.color = color;
	hit = -1;
	while (i < t->level->ssp[side].tri_amount)
	{
		if (0 < (tmp_dist = cast_face(t->level->ssp[side].tris[i], r, &tmp_color, t->img)) &&
			tmp_dist < res.dist)
		{
			if (!t->level->ssp[side].tris[i].opacity)
			{
				res.dist = tmp_dist;
				*res.color = tmp_color;
			}
			hit = i;
		}
		i++;
	}
	if (hit == -1)
		return (res.dist);
	if (t->level->ssp[side].tris[hit].opacity)
		cast_transparent(r, &t->level->ssp[side], t->img, &res);
	vec_mult(&r.dir, res.dist - 0.00001);
	if (t->level->ssp[side].tris[hit].reflectivity)
	{
		tmp_color = cast_reflection(r, t, t->level->ssp[side].tris[hit]);
		*res.color = crossfade((unsigned)*res.color >> 8,
			tmp_color >> 8, t->level->ssp[side].tris[hit].reflectivity * 0xff, 0);
	}
	if (t->level->sun_contrast || t->level->direct_shadow_contrast)
		*res.color = crossfade((unsigned)*res.color >> 8, t->level->shadow_color,
			rt_shadow(r, t, t->level->ssp[side].tris[hit]) * 0xff, 0);
	return (res.dist);
}

int			raycast(void *data_pointer)
{
	t_rthread	*t = data_pointer;
	t_ray		r;
	float		angle = t->level->look_side;
	int			pixel_gap = t->level->ui->raycast_quality;
	int			rand_amount = 0;

	if (t->level->ui->raycast_quality >= NOISE_QUALITY_LIMIT)
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
			if (!rand_amount || rand() % rand_amount)	//skip random pixel
			if (!(x % pixel_gap) && !(y % pixel_gap))
			{
				t->window->frame_buffer[x + (y * RES_X)] = 0;
				t->window->depth_buffer[x + (y * RES_X)] = 0;

				float ym = (1.0 / RES_Y * y - 0.5);	//multiply these to change fov
				float xm = (1.0 / RES_X * x - 0.5);

				r.dir.x = cam.x + up.x * ym + side.x * xm;
				r.dir.y = cam.y + up.y * ym + side.y * xm;
				r.dir.z = cam.z + up.z * ym + side.z * xm;

				int		*color = (int*)&t->window->frame_buffer[x + (y * RES_X)];
				float	*dist = &t->window->depth_buffer[x + (y * RES_X)];
				if (!t->level->ui->fog)
					*color = skybox(*t->level, r);
				*dist = cast_all_color(r, t, x >= RES_X / 2, color);
				if (t->level->ui->fog)
					*color = fog(*color, *dist, t->level->ui->fog_color, t->level);
			}
		}
	}
	return (0);
}
