/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raster.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 23:51:41 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/31 23:53:35by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void			ss_minmax(t_vec2 *min, t_vec2 *max, t_tri tri)
{
	min->x = tri.ss_verts[0].x;
	min->x = fmin(tri.ss_verts[1].x, min->x);
	min->x = fmin(tri.ss_verts[2].x, min->x);
	if (tri.isquad)
		min->x = fmin(tri.ss_verts[3].x, min->x);

	min->y = tri.ss_verts[0].y;
	min->y = fmin(tri.ss_verts[1].y, min->y);
	min->y = fmin(tri.ss_verts[2].y, min->y);
	if (tri.isquad)
		min->y = fmin(tri.ss_verts[3].y, min->y);

	max->x = tri.ss_verts[0].x;
	max->x = fmax(tri.ss_verts[1].x, max->x);
	max->x = fmax(tri.ss_verts[2].x, max->x);
	if (tri.isquad)
		max->x = fmax(tri.ss_verts[3].x, max->x);

	max->y = tri.ss_verts[0].y;
	max->y = fmax(tri.ss_verts[1].y, max->y);
	max->y = fmax(tri.ss_verts[2].y, max->y);
	if (tri.isquad)
		max->y = fmax(tri.ss_verts[3].y, max->y);

	min->x = fmax(0, min->x);
	min->y = fmax(0, min->y);

	max->x = fmin(RES_X, max->x);
	max->y = fmin(RES_Y, max->y);
}

t_vec3			ss_to_uv(t_vec2 fp, t_tri tri, t_bmp *txtr, int is_quad)
{
	t_vec2	diff;
	t_vec2	v0;
	t_vec2	v1;
	t_vec2	v2;
	t_vec3	uvw;

	if (is_quad)
		v0 = tri.verts[3].txtr;
	else
		v0 = tri.verts[0].txtr;
	v1 = tri.verts[1].txtr;
	v2 = tri.verts[2].txtr;
	uvw.z = fabs((fp.x * (v1.y - v2.y) +
				v1.x * (v2.y - fp.y) +
				v2.x * (fp.y - v1.y)));
	uvw.y = fabs((v0.x * (fp.y - v2.y) +
				fp.x * (v2.y - v0.y) +
				v2.x * (v0.y - fp.y)));
	uvw.x = fabs((v0.x * (v1.y - fp.y) +
				v1.x * (fp.y - v0.y) +
				fp.x * (v0.y - v1.y)));
	normalize_data(&uvw, tri);
	return (uvw);
}

int				raster(void *data_pointer)
{
	t_rthread		*t = data_pointer;
	t_cast_result	res;
	t_vec2			screen;
	t_vec3			uv;
	t_vec2			min;
	t_vec2			max;
	t_tri			tri;
	int				i;

	i = t->id;
	res.texture = &t->level->texture;
	res.baked = NULL;
	res.normal_map = NULL;

	while (i < t->level->visible.tri_amount)
	{
		tri = t->level->visible.tris[i];
		ss_minmax(&min, &max, tri);
		screen.x = min.x;
		while (screen.x < max.x)
		{
			screen.y = min.y;
			while (screen.y < max.y)
			{
				if (point_in_tri(screen, tri.ss_verts[0], tri.ss_verts[1], tri.ss_verts[2]))
				{
					uv = ss_to_uv(screen, tri, &t->level->texture, 0);
					face_color(uv.x, uv.y, tri, &res);
					t->window->frame_buffer[(int)screen.x + ((int)screen.y * RES_X)] = res.color;
				}
				else if (tri.isquad && (point_in_tri(screen, tri.ss_verts[3], tri.ss_verts[1], tri.ss_verts[2])))
				{
					uv = ss_to_uv(screen, tri, &t->level->texture, 1);
					face_color(uv.x, uv.y, tri, &res);
					t->window->frame_buffer[(int)screen.x + ((int)screen.y * RES_X)] = res.color;
				}
				screen.y++;
			}
			screen.x++;
		}
		i += THREAD_AMOUNT;
	}
	return (0);
}
