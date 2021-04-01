/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaders.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 16:52:44 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/01 00:01:09 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		transparency(t_ray r, t_obj *obj, t_bmp *texture, t_cast_result *res)
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

float		shadow(t_ray r, t_rthread *t, t_tri hit)
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

int			reflection(t_ray *r, t_rthread *t, t_tri hit, int depth)
{
	float			dist;
	float			tmp_dist;
	int				tmp_color;
	int				res_col;
	int				new_hit;
	int				i;

	t_ray	normal;
	vec_add(&normal.pos, r->dir, r->pos);
	vec_normalize(&r->dir);
	vec_cross(&normal.dir, hit.v0v1, hit.v0v2);
	vec_normalize(&normal.dir);
	vec_mult(&normal.dir, vec_dot(r->dir, normal.dir) * -2);
	vec_add(&r->dir, r->dir, normal.dir);
	vec_normalize(&r->dir);
	r->pos.x = normal.pos.x;
	r->pos.y = normal.pos.y;
	r->pos.z = normal.pos.z;
	dist = FLT_MAX;
	res_col = 0;
	i = 0;
	while (i < t->level->all.tri_amount)
	{
		if (0 < (tmp_dist = cast_face(t->level->all.tris[i], *r, &tmp_color, t->img)) &&
			tmp_dist < dist)
		{
			res_col = tmp_color;
			dist = tmp_dist;
			new_hit = i;
		}
		i++;
	}
	if (dist == FLT_MAX)
		return (skybox(*t->level, *r));
	if (depth < REFLECTION_DEPTH)
	{
		vec_mult(&r->dir, dist - 0.00001);
		res_col = crossfade((unsigned)res_col >> 8,
			reflection(r, t, t->level->all.tris[new_hit], depth + 1) >> 8,
			t->level->all.tris[new_hit].reflectivity * 0xff, 0);
	}
	return (res_col);
}

float	wave_shader(t_vec3 mod)
{
	float			time;
	float			oscillation;
	float			res;

	time = SDL_GetTicks() / 1000.0;
	oscillation = (sin(time) + 1) / 2;
	mod.x += sin(mod.z / 2) / 2 + (-sin(mod.z / 2) * oscillation * 4) / 4;
	mod.x += time;
	mod.x /= 2;
	if (mod.x < 0)
		mod.x = 2 - fmod(fabs(mod.x), 2);
	else
		mod.x = fmod(mod.x, 2);
	if (mod.x > 1)
		mod.x = 2 - mod.x;
	res = sin(mod.x + oscillation / 2);
	res = fmod(res, 1);
	return (res);
}
