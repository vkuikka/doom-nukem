/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spray.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/01 18:48:35 by vkuikka           #+#    #+#             */
/*   Updated: 2021/06/04 20:17:15 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

t_vec2		uv_to_2d(t_tri tri, t_bmp *txtr, t_vec3 uvw, int isquad)
{
	t_vec2		av0;
	t_vec2		av1;
	t_vec2		av2;
	t_vec2		res;

	if (isquad)
		av0 = tri.verts[3].txtr;
	else
		av0 = tri.verts[0].txtr;
	av1 = tri.verts[1].txtr;
	av2 = tri.verts[2].txtr;
	vec2_mult(&av0, uvw.z);
	vec2_mult(&av1, uvw.y);
	vec2_mult(&av2, uvw.x);
	res = av0;
	vec2_add(&res, res, av1);
	vec2_add(&res, res, av2);
	return (res);
}

static void		cast_uv(t_tri t, t_ray ray, t_vec3 *uvw)
{
	t_vec3	pvec;
	vec_cross(&pvec, ray.dir, t.v0v2);
	float invdet = 1 / vec_dot(pvec, t.v0v1); 

	t_vec3	tvec;
	vec_sub(&tvec, ray.pos, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;

	t_vec3 qvec;
	vec_cross(&qvec, tvec, t.v0v1);
	float v = vec_dot(ray.dir, qvec) * invdet;

	uvw->x = u;
	uvw->y = v;
	uvw->z = 1 - u - v;
}


static int		raycast_face_draw(t_ray r, t_level *l)
{
	t_cast_result	res;
	float			dist;
	float			tmp;
	int				i;
	int				new_hit;

	i = 0;
	dist = FLT_MAX;
	new_hit = 0;
	while (i < l->all.tri_amount)
	{
		tmp = cast_face(l->all.tris[i], r, &res);
		if (tmp > 0 && tmp < dist)
		{
			dist = tmp;
			new_hit = l->all.tris[i].index;
		}
		i++;
	}
	return (new_hit);
}

void		spray(t_camera *cam, t_level *level)
{
	t_vec3	point;
	t_vec3	uvw;
	t_vec2	pos;
	t_ray	r;
	int		hit;
	float	xm;
	float	ym;

	r.dir = cam->front;
	r.pos = cam->pos;
	ym = cam->fov_y / RES_Y * RES_Y / 2 - cam->fov_y / 2;
	xm = cam->fov_x / RES_X * RES_X / 2 - cam->fov_x / 2;
	// xm = 0.5;
	// ym = 0.5;
	r.dir.x += cam->up.x * ym + cam->side.x * xm;
	r.dir.y += cam->up.y * ym + cam->side.y * xm;
	r.dir.z += cam->up.z * ym + cam->side.z * xm;
	hit = raycast_face_draw(r, level);
	cast_uv(level->all.tris[hit], r, &uvw);
	pos = uv_to_2d(level->all.tris[hit], &level->texture, uvw, 0);
	pos.y = 1.0 - pos.y;
	pos.x *= level->texture.width;
	pos.y *= level->texture.height;
	// printf("%d %d\n", (int)pos.x, (int)pos.y);

	for (int i = pos.x - 10; i < pos.x + 10; i++)
		for (int j = pos.y - 10; j < pos.y + 10; j++)
			level->spray_overlay[i + j * level->texture.width] = 0xff0000ff;
	// level->spray_overlay[(int)pos.x + (int)pos.y * level->texture.width] = 0xff00ffff;
}
