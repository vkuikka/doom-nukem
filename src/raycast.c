/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/03 07:13:57 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	rot_cam(t_vec3 *cam, const float lon, const float lat)
{
	float		phi;
	float		radius;

	phi = (M_PI / 2 - lat);
	radius = 1;
	cam->x = radius * sin(phi) * cos(lon);
	cam->y = radius * cos(phi);
	cam->z = radius * sin(phi) * sin(lon);
}

static void	trace_bounce(t_cast_result *res, t_obj *obj, t_level *l)
{
	float	opacity_value;

	if (l->all.tris[res->face_index].reflectivity
		&& res->reflection_depth < REFLECTION_DEPTH)
	{
		res->reflection_depth++;
		if (res->reflection_depth == 1)
			reflection(res, l,
				&l->all.tris[res->face_index].reflection_obj_first_bounce);
		else
			reflection(res, l,
				&l->all.tris[res->face_index].reflection_obj_all);
	}
	opacity_value = 1.0 - (res->color << 24 >> 24) / (float)0xff;
	if (!opacity_value)
		opacity_value = l->all.tris[res->face_index].opacity;
	if (opacity_value && res->reflection_depth < REFLECTION_DEPTH)
	{
		res->reflection_depth++;
		opacity(res, l, obj, opacity_value);
	}
}

static void	raytrace(t_cast_result *res, t_obj *obj, t_level *l)
{
	t_vec3	face_normal;
	t_color	light;
	t_vec3	tmp;

	vec_add(&tmp, res->ray.dir, res->ray.pos);
	face_normal = l->all.tris[res->face_index].normal;
	vec_normalize(&res->normal);
	if (l->all.tris[res->face_index].shader == 1)
		res->color = shader_wave(tmp, &res->normal, 0x070C5A, 0x020540);
	if (l->all.tris[res->face_index].shader == 2)
		res->color = shader_rule30(tmp);
	else if (!res->baked || res->raytracing)
	{
		light = sunlight(l, res, lights(l, res, face_normal));
		res->color
			= brightness(res->color >> 8, light) + (res->color << 24 >> 24);
	}
	trace_bounce(res, obj, l);
}

static int	cast_loop(t_obj *obj, t_cast_result *res)
{
	float	tmp_dist;
	float	min_dist;
	int		new_hit;
	int		i;
	t_vec2	uv;

	i = 0;
	min_dist = FLT_MAX;
	new_hit = -1;
	while (i < obj->tri_amount)
	{
		tmp_dist = cast_face(obj->tris[i], res->ray, res);
		if (0 < tmp_dist && tmp_dist < min_dist
			&& obj->tris[i].index != res->face_index)
		{
			min_dist = tmp_dist;
			new_hit = i;
			uv = res->uv;
		}
		i++;
	}
	res->uv = uv;
	res->dist = min_dist;
	return (new_hit);
}

void	cast_all_color(t_level *l, t_obj *obj, t_cast_result *res)
{
	int		new_hit;

	new_hit = cast_loop(obj, res);
	if (new_hit == -1)
		res->color = skybox(l, *res);
	else
	{
		res->face_index = obj->tris[new_hit].index;
		face_color(res->uv.x, res->uv.y, obj->tris[new_hit], res);
		vec_mult(&res->ray.dir, res->dist);
		vec_add(&res->ray.pos, res->ray.pos, res->ray.dir);
		raytrace(res, obj, l);
	}
}

t_ray	ray_set(t_camera *cam, float fov, t_ivec2 xy)
{
	t_ray	res;
	float	xm;
	float	ym;

	xm = fov * ((float)RES_X / RES_Y);
	ym = fov;
	xm = xm / RES_X * xy.x - xm / 2;
	ym = ym / RES_Y * xy.y - ym / 2;
	res.dir.x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	res.dir.y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	res.dir.z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
	res.pos = cam->pos;
	return (res);
}

void	cast_result_set(t_cast_result *res, t_level *level)
{
	res->raytracing = level->ui.raytracing;
	res->normal_map = &level->normal_map;
	res->texture = &level->texture;
	res->spray_overlay = level->spray_overlay;
	if (level->bake_status != BAKE_NOT_BAKED)
		res->baked = level->baked;
	else
		res->baked = NULL;
	res->reflection_depth = 0;
	res->face_index = -1;
}

void	raycast(t_level *level, t_window *window, int thread_id)
{
	t_cast_result	res;
	t_ivec2			xy;

	xy.x = thread_id;
	while (xy.x < RES_X)
	{
		xy.y = -1;
		while (++xy.y < RES_Y)
		{
			if (!(xy.x % level->ui.raycast_quality)
				&& !(xy.y % level->ui.raycast_quality))
			{
				res.ray = ray_set(&level->cam, level->ui.fov, xy);
				cast_result_set(&res, level);
				cast_all_color(level, &level->ssp[get_ssp(xy)], &res);
				if (level->ui.fog)
					fog(&res.color, res.dist, level->ui.fog_color.color, level);
				window->frame_buffer[xy.x + (xy.y * RES_X)]
					= (res.color >> 8 << 8) + 0xff;
				window->depth_buffer[xy.x + (xy.y * RES_X)] = res.dist;
			}
		}
		xy.x += THREAD_AMOUNT;
	}
}

int	init_raycast(void *data_pointer)
{
	t_rthread	*thread;

	thread = data_pointer;
	raycast(thread->level, thread->window, thread->id);
	return (0);
}
