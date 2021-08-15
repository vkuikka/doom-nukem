/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raster.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 23:51:41 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/15 22:45:05 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void			ss_minmax(t_vec2 *min, t_vec2 *max, t_ss_tri tri, int resx, int resy)
{
	min->x = tri.verts[0].pos.x;
	min->x = fmin(tri.verts[1].pos.x, min->x);
	min->x = fmin(tri.verts[2].pos.x, min->x);

	min->y = tri.verts[0].pos.y;
	min->y = fmin(tri.verts[1].pos.y, min->y);
	min->y = fmin(tri.verts[2].pos.y, min->y);

	max->x = tri.verts[0].pos.x;
	max->x = fmax(tri.verts[1].pos.x, max->x);
	max->x = fmax(tri.verts[2].pos.x, max->x);

	max->y = tri.verts[0].pos.y;
	max->y = fmax(tri.verts[1].pos.y, max->y);
	max->y = fmax(tri.verts[2].pos.y, max->y);

	min->x = fmax(0, min->x);
	min->y = fmax(0, min->y);

	max->x = fmin(resx, max->x);
	max->y = fmin(resy, max->y);
}

t_vec3			ss_to_uv(t_vec2 *fp, t_ss_tri *tri, t_bmp *txtr)
{
	t_vec2	*v0;
	t_vec2	*v1;
	t_vec2	*v2;
	t_vec3	uvw;

	v0 = &tri->verts[0].pos;
	v1 = &tri->verts[1].pos;
	v2 = &tri->verts[2].pos;
	uvw.z = fabs((fp->x * (v1->y - v2->y) +
				v1->x * (v2->y - fp->y) +
				v2->x * (fp->y - v1->y)));
	uvw.y = fabs((v0->x * (fp->y - v2->y) +
				fp->x * (v2->y - v0->y) +
				v2->x * (v0->y - fp->y)));
	uvw.x = fabs((v0->x * (v1->y - fp->y) +
				v1->x * (fp->y - v0->y) +
				fp->x * (v0->y - v1->y)));
	normalize_data(&uvw);
	return (uvw);
}

unsigned int			raster_face_color(float u, float v, t_ss_tri t, t_bmp *texture)
{
	int		x;
	int 	y;
	float	w;

	w = 1 - u - v;
	x =	((t.verts[0].uv.x * texture->width * w +
			t.verts[1].uv.x * texture->width * v +
			t.verts[2].uv.x * texture->width * u) / (float)(u + v + w));
	y =	((t.verts[0].uv.y * texture->height * w +
			t.verts[1].uv.y * texture->height * v +
			t.verts[2].uv.y * texture->height * u) / (float)(u + v + w));
	wrap_coords(&x, &y, texture->width, texture->height);
	return(texture->image[x + (y * texture->width)]);
}

int				raster(void *data_pointer)
{
	t_rthread		*t = data_pointer;
	t_vec2			screen;
	t_vec3			uv;
	t_vec2			min;
	t_vec2			max;
	t_ss_tri		*all;
	t_ss_tri		*tri;
	int				i;

	int resx = RES_X / t->level->ui.raycast_quality;
	int resy = RES_Y / t->level->ui.raycast_quality;


	i = 0;
	all = &t->level->ss_tris[0];
	while (i < t->level->ss_tri_amount)
	{
		tri = &all[i];
		ss_minmax(&min, &max, *tri, resx, resy);
		if (min.x < resx / THREAD_AMOUNT * (t->id + 0))
			min.x = resx / THREAD_AMOUNT * (t->id + 0);
		if (max.x > resx / THREAD_AMOUNT * (t->id + 1))
			max.x = resx / THREAD_AMOUNT * (t->id + 1);
		screen.x = min.x;
		while (screen.x < max.x)
		{
			screen.y = min.y;
			while (screen.y < max.y)
			{
				if (!t->window->frame_buffer[(int)screen.x + ((int)screen.y * RES_X)]
				&& point_in_tri(screen, tri->verts[0].pos, tri->verts[1].pos, tri->verts[2].pos))
				{
					uv = ss_to_uv(&screen, tri, &t->level->texture);
					t->window->frame_buffer[(int)screen.x + ((int)screen.y * RES_X)]
					= raster_face_color(uv.x, uv.y, *tri, &t->level->texture);
				}
				screen.y++;
			}
			screen.x++;
		}
		i++;
	}
	return (0);
}
