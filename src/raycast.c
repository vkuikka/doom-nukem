/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/22 23:39:02 by vkuikka          ###   ########.fr       */
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

static void	raytrace(t_cast_result *res, t_obj *obj, t_level *l)
{
	float	opacity_value;
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
	if (l->all.tris[res->face_index].reflectivity
		&& res->reflection_depth < REFLECTION_DEPTH)
	{
		res->reflection_depth++;
		if (res->reflection_depth == 1)
			reflection(res, l,
				l->all.tris[res->face_index].reflection_obj_first_bounce);
		else
			reflection(res, l, l->all.tris[res->face_index].reflection_obj_all);
	}
	opacity_value = 1.0 - (res->color << 24 >> 24) / (float)0xff;
	if (!opacity_value)
		opacity_value = l->all.tris[res->face_index].opacity;
	if (opacity_value)
	{
		res->reflection_depth++;
		opacity(res, l, obj, opacity_value);
	}
}

void	cast_all_color(t_level *l, t_obj *obj, t_cast_result *res)
{
	float	dist;
	float	tmp_dist;
	int		new_hit;
	int		color;
	int		i;
	t_vec2	uv;

	dist = FLT_MAX;
	i = 0;
	color = 0;
	new_hit = -1;
	while (i < obj->tri_amount)
	{
		tmp_dist = cast_face(obj->tris[i], res->ray, res);
		if (0 < tmp_dist && tmp_dist < dist
			&& obj->tris[i].index != res->face_index)
		{
			dist = tmp_dist;
			new_hit = i;
			uv = res->uv;
		}
		i++;
	}
	res->dist = dist;
	if (new_hit == -1)
	{
		if (res->reflection_depth)
			res->color = skybox(l, &l->sky.all, res->ray);
		else
			res->color = skybox(l, &l->sky.visible, res->ray);
	}
	else
	{
		res->uv = uv;
		res->face_index = obj->tris[new_hit].index;
		face_color(res->uv.x, res->uv.y, obj->tris[new_hit], res);
		vec_mult(&res->ray.dir, dist);
		vec_add(&res->ray.pos, res->ray.pos, res->ray.dir);
		raytrace(res, obj, l);
	}
}

t_ray	ray_set(t_camera *cam, float xm, float ym)
{
	t_ray	res;

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

int	raycast(void *data_pointer)
{
	int				rand_amount;
	int				pixel_gap;
	float			fov_x;
	t_cast_result	res;
	t_rthread		*t;
	int				x;
	int				y;

	t = data_pointer;
	pixel_gap = t->level->ui.raycast_quality;
	rand_amount = 0;
	if (t->level->ui.raycast_quality >= NOISE_QUALITY_LIMIT)
	{
		srand(SDL_GetTicks());
		rand_amount = 2;
	}
	fov_x = t->level->ui.fov * ((float)RES_X / RES_Y);
	x = t->id;
	while (x < RES_X)
	{
		y = -1;
		while (++y < RES_Y)
			if ((!rand_amount || rand() % rand_amount)
				&& !(x % pixel_gap) && !(y % pixel_gap))
			{
				res.ray = ray_set(&t->level->cam, fov_x / RES_X * x - fov_x / 2,
					t->level->ui.fov / RES_Y * y - t->level->ui.fov / 2);
				cast_result_set(&res, t->level);
				cast_all_color(t->level,
					&t->level->ssp[get_ssp_index(x, y)], &res);
				if (t->level->ui.fog)
					res.color = fog(res.color, res.dist,
							t->level->ui.fog_color, t->level);
				t->window->frame_buffer[x + (y * RES_X)]
					= (res.color >> 8 << 8) + 0xff;
				t->window->depth_buffer[x + (y * RES_X)] = res.dist;
			}
		x += THREAD_AMOUNT;
	}
	return (0);
}
