/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaders.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 16:52:44 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/21 19:51:29by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		opacity(t_cast_result *res, t_level *l, t_obj *obj, float opacity)
{
	t_cast_result	transparent;
	t_ray			normal;

	transparent = *res;
	normal.pos = res->ray.pos;
	normal.dir = res->normal;
	vec_mult(&normal.dir, vec_dot(transparent.ray.dir, normal.dir) * l->all.tris[res->face_index].refractivity);
	vec_add(&transparent.ray.dir, transparent.ray.dir, normal.dir);
	cast_all_color(transparent.ray, l, obj, &transparent);
	res->color = crossfade((unsigned)res->color >> 8, (unsigned)transparent.color >> 8, opacity * 0xff, opacity * 0xff);
}

void		shadow(t_level *l, t_vec3 normal, t_cast_result *res)
{
	float	darkness;
	int		i;
	t_ray	r;

	if (vec_dot(normal, l->ui.sun_dir) < 0)
	{
		darkness = l->ui.direct_shadow_contrast;
		res->color = crossfade((unsigned)res->color >> 8, l->shadow_color, darkness * 0xff, (unsigned)res->color << 24 >> 24);
		return;
	}
	r.dir.x = l->ui.sun_dir.x;
	r.dir.y = l->ui.sun_dir.y;
	r.dir.z = l->ui.sun_dir.z;
	r.pos = res->ray.pos;
	i = 0;
	while (i < l->all.tris[res->face_index].shadow_faces->tri_amount)
	{
		if (0 < cast_face(l->all.tris[res->face_index].shadow_faces->tris[i], r, NULL))
		{
			darkness = l->ui.direct_shadow_contrast;
			res->color = crossfade((unsigned)res->color >> 8, l->shadow_color, darkness * 0xff, (unsigned)res->color << 24 >> 24);
			return;
		}
		i++;
	}
	darkness = (1 - vec_dot(normal, l->ui.sun_dir)) * l->ui.sun_contrast;
	res->color = crossfade((unsigned)res->color >> 8, l->shadow_color, darkness * 0xff, (unsigned)res->color << 24 >> 24);
}

void		lights(t_level *l, t_vec3 normal, t_cast_result *res)
{
	t_vec3	diff;
	float	dist;
	float	bright_value;
	int		i;

	i = 0;
	// if (l->ui.sun_contrast || l->ui.direct_shadow_contrast)
	// 	shadow(l, res->normal, res);
	bright_value = 0;
	while (i < l->light_amount)
	{
		vec_sub(&diff, res->ray.pos, l->lights[i].pos);
		dist = vec_length(diff);
		if (dist < l->lights[i].radius)
			bright_value += (1.0 - dist / l->lights[i].radius) * l->lights[i].brightness;
		i++;
	}
	bright_value += l->brightness;
	res->color = brightness((unsigned)res->color >> 8, bright_value, (unsigned)res->color << 24 >> 24);
}

void		reflection(t_cast_result *res, t_level *l, t_obj *obj)
{
	t_cast_result	reflection;
	t_ray			normal;

	reflection = *res;
	normal.pos = res->ray.pos;
	normal.dir = res->normal;
	vec_mult(&normal.dir, vec_dot(reflection.ray.dir, normal.dir) * -2);
	vec_add(&reflection.ray.dir, reflection.ray.dir, normal.dir);
	cast_all_color(reflection.ray, l, obj, &reflection);
	res->color = crossfade((unsigned)res->color >> 8, reflection.color >> 8, l->all.tris[res->face_index].reflectivity * 0xff, (unsigned)res->color << 24 >> 24);
}

unsigned		wave_shader(t_vec3 mod, t_vec3 *normal, unsigned col1, unsigned col2)
{
	float			time;
	float			oscillation;
	float			res;

	time = SDL_GetTicks() / 1000.0;
	oscillation = (sin(time) + 1) / 2;
	float	tmp = 2 * M_PI / 6 * (sin(mod.z) / 5 + sin(mod.x) / 4 + mod.x + time);
	tmp = sin(tmp);
	res = fabs(tmp);
	normal->x -= res / 5;
	normal->y = 1;
	normal->z = 0;
	vec_normalize(normal);
	col1 = crossfade(col1, col2, res * 0xff, 0xff);
	return (col1);
}
