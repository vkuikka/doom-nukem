/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spray.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/01 18:48:35 by vkuikka           #+#    #+#             */
/*   Updated: 2021/06/05 13:56:18 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

t_vec2		uv_to_2d(t_tri tri, t_bmp *txtr, t_vec2 uv, int isquad)
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
	vec2_mult(&av0, 1 - uv.x - uv.y);
	vec2_mult(&av1, uv.y);
	vec2_mult(&av2, uv.x);
	res = av0;
	vec2_add(&res, res, av1);
	vec2_add(&res, res, av2);
	return (res);
}

// static void		cast_uv(t_tri t, t_ray ray, t_vec3 *uvw)
// {
// 	t_vec3	pvec;
// 	vec_cross(&pvec, ray.dir, t.v0v2);
// 	float invdet = 1 / vec_dot(pvec, t.v0v1); 

// 	t_vec3	tvec;
// 	vec_sub(&tvec, ray.pos, t.verts[0].pos);
// 	float u = vec_dot(tvec, pvec) * invdet;

// 	t_vec3 qvec;
// 	vec_cross(&qvec, tvec, t.v0v1);
// 	float v = vec_dot(ray.dir, qvec) * invdet;

// 	uvw->x = u;
// 	uvw->y = v;
// 	uvw->z = 1 - u - v;
// }

static int		raycast_face_pos(t_ray r, t_level *l, t_obj *object, t_vec2 *uv)
{
	t_cast_result	res;
	float			dist;
	float			tmp;
	int				i;
	int				new_hit;

	i = 0;
	dist = FLT_MAX;
	new_hit = -1;
	while (i < object->tri_amount)
	{
		tmp = cast_face(object->tris[i], r, &res);
		if (tmp > 0 && tmp < dist)
		{
			dist = tmp;
			new_hit = object->tris[i].index;
		}
		i++;
	}
	if (new_hit == -1)
		return (-1);
	uv->x = res.u;
	uv->y = res.v;
	*uv = uv_to_2d(l->all.tris[new_hit], &l->texture, *uv, 0);
	return (new_hit);
}

// void		decal_overlay(t_level *level, int *frame_buffer)
// {
// 	int		x;
// 	int		y;
// 	float	per;
// 	int		i;

// 	x = 1/4 * RES_Y;
// 	while (x < 3/4 * RES_X)
// 	{
// 		y = 1/4 * RES_X;
// 		while (y < 3/4 * RES_X)
// 		{
// 			// frame_buffer[x + y * RES_X] = level->spray.image[]
// 			y++;
// 		}
// 		x++;
// 	}
// }

void		spray(t_camera *cam, t_level *level)
{
	t_vec3	point;
	t_vec2	uv;
	t_vec2	pos;
	t_ray	r;
	int		hit;
	float	xm;
	float	ym;

	r.dir = cam->front;
	r.pos = cam->pos;
	// ym = cam->fov_y / RES_Y * RES_Y / 2 - cam->fov_y / 2;
	// xm = cam->fov_x / RES_X * RES_X / 2 - cam->fov_x / 2;
	// xm = 0.5;
	// ym = 0.5;
	// r.dir.x += cam->up.x * ym + cam->side.x * xm;
	// r.dir.y += cam->up.y * ym + cam->side.y * xm;
	// r.dir.z += cam->up.z * ym + cam->side.z * xm;

	// hit = raycast_face_draw(r, level);
	// cast_uv(level->all.tris[hit], r, &uvw);
	// pos = uv_to_2d(level->all.tris[hit], &level->texture, uvw, 0);
	// pos.y = 1.0 - pos.y;
	// pos.x *= level->texture.width;
	// pos.y *= level->texture.height;
	float fov_x = level->ui.fov * ((float)RES_X / RES_Y);
	for (int x = 0; x < RES_X; x++)
	{
		for (int y = 0; y < RES_Y; y++)
		{
			float ym = level->ui.fov / RES_Y * y - level->ui.fov / 2;
			float xm = fov_x / RES_X * x - fov_x / 2;

			r.dir.x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
			r.dir.y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
			r.dir.z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
			// printf("%d %d\n", x, y);
			hit = raycast_face_pos(r, level, &level->ssp[get_ssp_index(x, y)], &uv);
			if (hit != -1)
			{
				pos.x = uv.x;
				pos.y = 1 - uv.y;
				// printf("%f %f -> ", pos.x, pos.y);
				pos.x *= level->texture.width;
				pos.y *= level->texture.height;
				if ((int)pos.x < level->texture.width && (int)pos.y < level->texture.height)
					level->spray_overlay[(int)pos.x + (int)pos.y * level->texture.width] = 0xff0000ff;
			}
		}
	}

	// for (int i = pos.x - 10; i < pos.x + 10; i++)
	// 	for (int j = pos.y - 10; j < pos.y + 10; j++)
	// 		level->spray_overlay[i + j * level->texture.width] = 0xff0000ff;
	// level->spray_overlay[(int)pos.x + (int)pos.y * level->texture.width] = 0xff00ffff;
}
