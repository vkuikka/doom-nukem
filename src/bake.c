/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bake.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 10:29:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/25 21:11:25 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	texture_minmax(t_vec2 *min, t_vec2 *max, t_tri tri)
{
	min->x = tri.verts[0].txtr.x;
	min->x = fmin(tri.verts[1].txtr.x, min->x);
	min->x = fmin(tri.verts[2].txtr.x, min->x);
	min->y = 1 - tri.verts[0].txtr.y;
	min->y = fmin(1 - tri.verts[1].txtr.y, min->y);
	min->y = fmin(1 - tri.verts[2].txtr.y, min->y);
	max->x = tri.verts[0].txtr.x;
	max->x = fmax(tri.verts[1].txtr.x, max->x);
	max->x = fmax(tri.verts[2].txtr.x, max->x);
	max->y = 1 - tri.verts[0].txtr.y;
	max->y = fmax(1 - tri.verts[1].txtr.y, max->y);
	max->y = fmax(1 - tri.verts[2].txtr.y, max->y);
}

void	normalize_data(t_vec3 *uvw)
{
	vec_div(uvw, uvw->x + uvw->y + uvw->z);
}

void	get_uv(t_vec3 *uvw, t_ivec2 ipoint, t_tri tri, t_bmp *txtr)
{
	t_vec2	fp;
	t_vec2	v0;
	t_vec2	v1;
	t_vec2	v2;

	fp.x = (float)ipoint.x / txtr->width;
	fp.y = 1 - (float)ipoint.y / txtr->height;
	if (!point_in_tri(fp, tri.verts[0].txtr,
			tri.verts[1].txtr, tri.verts[2].txtr))
		v0 = tri.verts[3].txtr;
	else
		v0 = tri.verts[0].txtr;
	v1 = tri.verts[1].txtr;
	v2 = tri.verts[2].txtr;
	uvw->z = fabs((fp.x * (v1.y - v2.y)
				+ v1.x * (v2.y - fp.y)
				+ v2.x * (fp.y - v1.y)));
	uvw->y = fabs((v0.x * (fp.y - v2.y)
				+ fp.x * (v2.y - v0.y)
				+ v2.x * (v0.y - fp.y)));
	uvw->x = fabs((v0.x * (v1.y - fp.y)
				+ v1.x * (fp.y - v0.y)
				+ fp.x * (v0.y - v1.y)));
	normalize_data(uvw);
}

t_vec3	uv_to_3d(t_tri tri, t_bmp *txtr, t_ivec2 point)
{
	t_vec3	uvw;
	t_vec3	av0;
	t_vec3	av1;
	t_vec3	av2;
	t_vec2	fp;

	get_uv(&uvw, point, tri, txtr);
	fp.x = (float)point.x / txtr->width;
	fp.y = 1 - (float)point.y / txtr->height;
	if (!point_in_tri(fp, tri.verts[0].txtr,
			tri.verts[1].txtr, tri.verts[2].txtr))
		av0 = tri.verts[3].pos;
	else
		av0 = tri.verts[0].pos;
	av1 = tri.verts[1].pos;
	av2 = tri.verts[2].pos;
	vec_mult(&av0, uvw.z);
	vec_mult(&av1, uvw.y);
	vec_mult(&av2, uvw.x);
	vec_add(&av0, av0, av1);
	vec_add(&av0, av0, av2);
	return (av0);
}

static void	wrap_coords(int *x, int *y, int max_x, int max_y)
{
	while (*y < 0)
		*y += max_y;
	if (*y >= max_y)
		*y = *y % max_y;
	while (*x < 0)
		*x += max_x;
	if (*x >= max_x)
		*x = *x % max_x;
}

static void	bake_pixel(t_level *l, t_ivec2 wrap, t_ivec2 i, t_cast_result *res)
{
	wrap_coords(&wrap.x, &wrap.y, l->texture.width, l->texture.height);
	if (!(l->baked[wrap.x + wrap.y * l->texture.width].r)
		&& !(l->baked[wrap.x + wrap.y * l->texture.width].g)
		&& !(l->baked[wrap.x + wrap.y * l->texture.width].b))
	{
		res->ray.pos = uv_to_3d(l->all.tris[res->face_index], &l->texture, i);
		res->normal = get_normal(l->normal_map.image[wrap.x
				+ (wrap.y * l->normal_map.width)]);
		vec_normalize(&res->normal);
		l->baked[wrap.x + wrap.y * l->texture.width] = sunlight(l, res,
				lights(l, res, l->all.tris[res->face_index].normal));
	}
}

static int	point_in_face(t_tri tri, t_vec2 point)
{
	return ((tri.isquad && point_in_tri(point, tri.verts[3].txtr,
				tri.verts[1].txtr, tri.verts[2].txtr))
		|| point_in_tri(point, tri.verts[0].txtr,
			tri.verts[1].txtr, tri.verts[2].txtr));
}

static void	bake_area(t_level *l, t_cast_result *res, t_ivec2 i)
{
	t_vec2	tmp;
	t_ivec2	wrap;
	t_tri	tri;

	wrap = i;
	wrap_coords(&wrap.x, &wrap.y, l->texture.width, l->texture.height);
	tmp.x = (float)i.x / l->texture.width;
	tmp.y = 1 - (float)i.y / l->texture.height;
	tri = l->all.tris[res->face_index];
	if (point_in_face(tri, tmp))
	{
		wrap_coords(&wrap.x, &wrap.y, l->texture.width, l->texture.height);
		res->ray.pos = uv_to_3d(l->all.tris[res->face_index], &l->texture, i);
		res->normal = get_normal(l->normal_map.image[wrap.x
				+ (wrap.y * l->normal_map.width)]);
		vec_normalize(&res->normal);
		l->baked[wrap.x + wrap.y * l->texture.width] = sunlight(l, res,
				lights(l, res, l->all.tris[res->face_index].normal));
		wrap.y -= 1;
		bake_pixel(l, wrap, i, res);
		wrap.x -= 1;
		bake_pixel(l, wrap, i, res);
		wrap.y += 1;
		bake_pixel(l, wrap, i, res);
	}
}

static void	bake_face(t_cast_result *res, t_level *l)
{
	t_ivec2			i;
	t_vec2			min;
	t_vec2			max;

	texture_minmax(&min, &max, l->all.tris[res->face_index]);
	i.x = min.x * l->texture.width;
	i.y = 0;
	while (i.x < max.x * l->texture.width)
	{
		i.y = min.y * l->texture.height;
		while (i.y < max.y * l->texture.height)
		{
			if (l->bake_status != BAKE_BAKING)
				return ;
			bake_area(l, res, i);
			i.y++;
		}
		i.x++;
	}
}

int	bake(void *d)
{
	t_level			*l;
	t_cast_result	res;
	int				tri;

	l = d;
	tri = 0;
	res.raytracing = TRUE;
	while (tri < l->all.tri_amount)
	{
		res.face_index = tri;
		bake_face(&res, l);
		tri++;
		l->bake_progress = 100 * (float)tri / (float)l->all.tri_amount;
	}
	if (l->bake_status != BAKE_NOT_BAKED)
		l->bake_status = BAKE_BAKED;
	return (1);
}

void	clear_bake(t_level *level)
{
	int	x;
	int	y;

	x = 0;
	while (x < level->texture.width)
	{
		y = 0;
		while (y < level->texture.height)
		{
			level->baked[x + y * level->texture.width].r = 0;
			level->baked[x + y * level->texture.width].g = 0;
			level->baked[x + y * level->texture.width].b = 0;
			y++;
		}
		x++;
	}
}

void	start_bake(t_level *level)
{
	if (level->texture.width != level->normal_map.width
		|| level->texture.height != level->normal_map.height)
	{
		nonfatal_error("texture and normal map are not same size");
		return ;
	}
	if (level->baked_size.x != level->texture.width
		|| level->baked_size.y != level->texture.height)
		level->baked = ft_realloc(level->baked,
				sizeof(t_color) * level->baked_size.x * level->baked_size.y,
				sizeof(t_color) * level->texture.height * level->texture.width);
	if (level->bake_status == BAKE_NOT_BAKED)
	{
		clear_bake(level);
		level->bake_status = BAKE_BAKING;
		level->bake_progress = 0;
		SDL_CreateThread(bake, "asd", (void *)level);
		level->baked_size.x = level->texture.width;
		level->baked_size.y = level->texture.height;
	}
}
