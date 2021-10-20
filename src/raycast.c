/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/20 22:05:09 by vkuikka          ###   ########.fr       */
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
	t_color	light;
	t_vec3	tmp;

	tmp = res->ray.pos;
	vec_normalize(&res->normal);
	if (l->all.tris[res->face_index].shader == SHADER_PERLIN)
		res->color = shader_perlin(tmp, l, res);
	if (l->all.tris[res->face_index].shader == SHADER_RULE_30)
		res->color = shader_rule30(tmp);
	else if (!res->baked || res->raytracing || obj->tris[res->face_index].isgrid
		|| obj->tris[res->face_index].shader != SHADER_NONE)
	{
		light = sunlight(l, res, lights(l, res));
		res->color
			= brightness(res->color >> 8, light) + (res->color << 24 >> 24);
		if (light.r - l->world_brightness > l->ui.bloom_limit
			|| light.g - l->world_brightness > l->ui.bloom_limit
			|| light.b - l->world_brightness > l->ui.bloom_limit)
		{
			res->light.r += light.r - l->world_brightness;
			res->light.g += light.g - l->world_brightness;
			res->light.b += light.b - l->world_brightness;
		}
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

void	cast_all_color(t_level *l, t_obj *obj, t_cast_result *res,
							int apply_fog)
{
	int		new_hit;

	vec_normalize(&res->ray.dir);
	new_hit = cast_loop(obj, res);
	if (new_hit == -1 && l->render_is_first_pass)
	{
		res->color = 0;
		return ;
	}
	if (new_hit == -1)
		skybox(l, res);
	else
	{
		res->face_index = obj->tris[new_hit].index;
		res->texture = obj->tris[new_hit].texture;
		face_color(res->uv.x, res->uv.y, obj->tris[new_hit], res);
		vec_mult(&res->ray.dir, res->dist);
		vec_add(&res->ray.pos, res->ray.pos, res->ray.dir);
		raytrace(res, obj, l);
	}
	if (l->ui.fog && apply_fog)
		fog(&res->color, res->dist, l->ui.fog_color.color, l);
}

t_ray	ray_set(t_camera *cam, t_ivec2 xy)
{
	t_ray	res;
	float	xm;
	float	ym;

	xm = cam->fov_x;
	ym = cam->fov_y;
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
	if (!level->ui.normal_map_disabled)
		res->normal_map = &level->normal_map;
	else
		res->normal_map = NULL;
	res->spray_overlay = level->spray_overlay;
	if (level->bake_status != BAKE_NOT_BAKED)
		res->baked = level->baked;
	else
		res->baked = NULL;
	res->reflection_depth = 0;
	res->face_index = -1;
	res->light.r = 0;
	res->light.g = 0;
	res->light.b = 0;
}

void	set_render_result(t_window *window, t_cast_result res, unsigned int i)
{
	window->frame_buffer[i] = res.color;
	window->depth_buffer[i] = res.dist;
	window->brightness_buffer[i] = res.light;
	window->normal_buffer[i] = res.normal;
}

int	raycast(t_level *level, t_window *window, int thread_id)
{
	t_cast_result	res;
	t_ivec2			xy;

	xy.x = thread_id * level->ui.raycast_quality;
	res.raycast_amount = 0;
	while (xy.x < RES_X)
	{
		xy.y = 0;
		while (xy.y < RES_Y)
		{
			if (!level->render_is_first_pass
				&& window->frame_buffer[xy.x + xy.y * RES_X])
				continue ;
			res.ray = ray_set(&level->cam, xy);
			cast_result_set(&res, level);
			cast_all_color(level, &level->ssp[get_ssp(xy)], &res, TRUE);
			set_render_result(window, res, xy.x + xy.y * RES_X);
			xy.y += level->ui.raycast_quality;
		}
		xy.x += THREAD_AMOUNT * level->ui.raycast_quality;
	}
	return (res.raycast_amount);
}

int	init_raycast(void *data_pointer)
{
	t_rthread	*thread;

	thread = data_pointer;
	return (raycast(thread->level, thread->window, thread->id));
}
