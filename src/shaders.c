/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaders.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 16:52:44 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/03 21:42:30 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		transparency(t_ray r, t_obj *obj, t_bmp *txtr, t_cast_result *res)
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
			0 < (tmp_dist = cast_face(obj->tris[i], r, &tmp_color, txtr)) &&
			tmp_dist < res->dist && tmp_dist > res->transparent_dist)
		{
			transparent_face = i;
			res->transparent_dist = tmp_dist;
			res->transparent_color = tmp_color;
		}
		if (++i == obj->tri_amount && res->transparent_dist && !(i = 0))
		{
			res->dist = res->transparent_dist;
			res->color = crossfade((unsigned)res->color >> 8,
res->transparent_color >> 8, obj->tris[transparent_face].opacity * 0xff);
			res->transparent_dist = 0;
		}
	}
}

float		shadow(t_ray r, t_rthread *t, t_vec3 normal)
{
	int				direct_shadow;
	int				i;

	vec_add(&r.pos, r.dir, r.pos);
	r.dir.x = t->level->ui->sun_dir.x;
	r.dir.y = t->level->ui->sun_dir.y;
	r.dir.z = t->level->ui->sun_dir.z;
	direct_shadow = 0;
	i = 0;
	while (i < t->level->all.tri_amount && !direct_shadow)
	{
		if (0 > cast_face(t->level->all.tris[i], r, NULL, NULL))
			direct_shadow = 1;
		i++;
	}
	if (!direct_shadow)
		return ((1 - vec_dot(normal, t->level->ui->sun_dir)) * t->level->ui->sun_contrast);
	return (t->level->ui->direct_shadow_contrast);
}

int			reflection(t_ray *r, t_rthread *t, t_vec3 normal_dir, int depth)
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
	normal.dir = normal_dir;
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
		if (0 < (tmp_dist = cast_face(t->level->all.tris[i], *r, &tmp_color, &t->level->texture)) &&
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
			reflection(r, t, t->level->all.tris[new_hit].normal, depth + 1) >> 8,
			t->level->all.tris[new_hit].reflectivity * 0xff);
	}
	return (res_col);
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
	col1 = crossfade(col1, col2, res * 0xff);
	return (col1);
}
