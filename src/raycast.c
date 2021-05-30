/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/18 18:25:74vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

float			cast_face(t_tri t, t_ray ray, t_cast_result *res)
{
	t_vec3	pvec;
	vec_cross(&pvec, ray.dir, t.v0v2);
	float invdet = 1 / vec_dot(pvec, t.v0v1); 

	t_vec3	tvec;
	vec_sub(&tvec, ray.pos, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;
	if (t.isgrid)
	{
		if (u < 0)
			u = 1 - fabs(u - (int)u);
		else
			u = fabs(u - (int)u);
	}
	if (u < 0 || u > 1)
		return 0;
	t_vec3 qvec;
	vec_cross(&qvec, tvec, t.v0v1);
	float v = vec_dot(ray.dir, qvec) * invdet;
	if (t.isgrid)
	{
		if (v < 0)
			v = 1 - fabs(v - (int)v);
		else
			v = fabs(v - (int)v);
	}
	if (t.isquad)
	{
		if (v < 0 || v > 1) //quad hack
			return 0;
	}
	else if (v < 0 || u + v > 1)
		return 0;
	float dist = vec_dot(qvec, t.v0v2) * invdet;
	if (res)
	{
		res->u = u;
		res->v = v;
		res->dist = dist;
	}
	return (dist);
}

void			rot_cam(t_vec3 *cam, const float lon, const float lat)
{
	const float	phi = (M_PI / 2 - lat);
	const float	theta = lon;
	float radius = 1;

	cam->x = radius * sin(phi) * cos(theta);
	cam->y = radius * cos(phi);
	cam->z = radius * sin(phi) * sin(theta);
}

static void		raytrace(t_cast_result *res, t_obj *obj, t_ray r, t_level *l)
{
	float		opacity_value;

	vec_normalize(&res->normal);
	res->ray = r;
	if (l->all.tris[res->face_index].shader == 1)
	{
		t_vec3 tmp;
		vec_add(&tmp, r.dir, r.pos);
		res->color = wave_shader(tmp, &res->normal, 0x070C5A, 0x020540);
	}
	if (!res->baked || res->raytracing)
		res->color = brightness(res->color >> 8, lights(l, res->ray.pos, res->normal, res->raytracing)) + (res->color << 24 >> 24);
	if (l->all.tris[res->face_index].reflectivity &&
		res->reflection_depth < REFLECTION_DEPTH)
	{
		res->reflection_depth++;
		if (res->reflection_depth == 1)
			reflection(res, l, l->all.tris[res->face_index].reflection_obj_first_bounce);
		else
			reflection(res, l, l->all.tris[res->face_index].reflection_obj_all);
	}
	opacity_value = 1.0 - (res->color << 24 >> 24) / (float)0xff;
	if (!opacity_value)
		opacity_value = l->all.tris[res->face_index].opacity;
	if (opacity_value)
	{
		res->reflection_depth++;
		opacity(res, l, l->all.tris[res->face_index].opacity_obj_all, opacity_value);
	}
}

void			cast_all_color(t_ray r, t_level *l, t_obj *obj, t_cast_result *res)
{
	float			dist;
	int				new_hit;
	int				color;
	int				i;
	float			u;
	float			v;

	dist = FLT_MAX;
	res->dist = FLT_MAX;
	i = 0;
	color = 0;
	new_hit = -1;
	while (i < obj->tri_amount)
	{
		if (0 < cast_face(obj->tris[i], r, res) && res->dist < dist && obj->tris[i].index != res->face_index)
		{
			dist = res->dist;
			new_hit = i;
			u = res->u;
			v = res->v;
		}
		i++;
	}
	res->dist = dist;
	if (new_hit == -1)
	{
		if (l->skybox_brightness == 0)
			res->color = skybox(&l->sky.img, res->reflection_depth ? &l->sky.all : &l->sky.visible, r, l->brightness);
		else
			res->color = skybox(&l->sky.img, res->reflection_depth ? &l->sky.all : &l->sky.visible, r, l->skybox_brightness);
	}
	else
	{
		res->u = u;
		res->v = v;
		res->face_index = obj->tris[new_hit].index;
		face_color(res->u, res->v, obj->tris[new_hit], res);
		vec_mult(&r.dir, dist);
		vec_add(&r.pos, r.pos, r.dir);
		raytrace(res, obj, r, l);
	}
}

int				raycast(void *data_pointer)
{
	t_rthread	*t = data_pointer;
	t_ray		r;
	t_camera	*cam = &t->level->cam;
	int			pixel_gap = t->level->ui.raycast_quality;
	int			rand_amount = 0;

	if (t->level->ui.raycast_quality >= NOISE_QUALITY_LIMIT)
	{
		srand(SDL_GetTicks());
		rand_amount = 2;
	}
	r.pos.x = cam->pos.x;
	r.pos.y = cam->pos.y;
	r.pos.z = cam->pos.z;
	float fov_x = t->level->ui.fov * ((float)RES_X / RES_Y);
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
				float ym = t->level->ui.fov / RES_Y * y - t->level->ui.fov / 2;
				float xm = fov_x / RES_X * x - fov_x / 2;

				r.dir.x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
				r.dir.y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
				r.dir.z = cam->front.z + cam->up.z * ym + cam->side.z * xm;

				t_cast_result	res;
				res.raytracing = t->level->ui.raytracing;
				res.normal_map = &t->level->normal_map;
				res.texture = &t->level->texture;
				if (t->level->bake_status != BAKE_NOT_BAKED)
					res.baked = t->level->baked;
				else
					res.baked = NULL;
				res.reflection_depth = 0;
				res.face_index = -1;
				cast_all_color(r, t->level, &t->level->ssp[get_ssp_index(x, y)], &res);
				if (t->level->ui.fog)
					res.color = fog(res.color, res.dist, t->level->ui.fog_color, t->level);
				t->window->frame_buffer[x + (y * RES_X)] = (res.color >> 8 << 8) + 0xff;
				t->window->depth_buffer[x + (y * RES_X)] = res.dist;
			}
		}
	}
	return (0);
}
