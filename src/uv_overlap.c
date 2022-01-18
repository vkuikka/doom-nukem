/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_overlap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsjoberg <lsjoberg@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 13:48:01 by vkuikka           #+#    #+#             */
/*   Updated: 2022/01/18 16:32:37 by lsjoberg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	div_every_uv(t_level *l)
{
	int		i;
	int		j;
	t_vert	*vert;

	i = 0;
	while (i < l->all.tri_amount)
	{
		j = 0;
		while (j < 3 + l->all.tris[i].isquad)
		{
			vert = &l->all.tris[i].verts[j];
			vert->txtr.y = 0.5 + vert->txtr.y / 2;
			j++;
		}
		i++;
	}
}

/*
**	Checks 3x3 pixels around pixel corner for triangle intersection
**	3x3 square is done because vectors can go inside image pixels and then
**	intersection will not be found
*/

static void	coloring_loop(t_tri *t1, t_bmp *img, t_uv *v)
{
	while (++v->inc.y < 3)
	{
		v->check.x = v->coord_uv.x + v->precision.x * v->inc.x;
		v->check.y = v->coord_uv.y + v->precision.y * v->inc.y;
		if (point_in_tri(v->check,
				t1->verts[0].txtr, t1->verts[1].txtr, t1->verts[2].txtr)
			|| (t1->isquad && point_in_tri(v->check, t1->verts[3].txtr,
					t1->verts[1].txtr, t1->verts[2].txtr)))
		{
			v->og.x = v->coord.x - v->diff.x * img->width;
			v->og.y = (img->height - v->coord.y) + v->diff.y * img->height;
			img->image[v->coord.x + (img->height - v->coord.y) * img->width]
				= img->image[v->og.x + v->og.y * img->width];
			return ;
		}
	}
}

static void	check_and_color(t_tri *t1, t_bmp *img, t_uv *v)
{
	v->precision.x = 1 / (float)img->width;
	v->precision.y = 1 / (float)img->height;
	v->coord_uv.x = v->coord.x / (float)img->width - v->precision.x;
	v->coord_uv.y = v->coord.y / (float)img->height - v->precision.y;
	v->inc.x = -1;
	while (++v->inc.x < 3)
	{
		v->inc.y = -1;
		coloring_loop(t1, img, v);
	}
}

void	copy_uv2(t_tri *t1, t_uv	*v, t_bmp *img)
{
	if (t1->isquad)
		v->max_y = fmax(v->max_y, t1->verts[3].txtr.y);
	v->min_x *= (float)img->width;
	v->max_x *= (float)img->width;
	v->max_y *= (float)img->height;
	v->min_y *= (float)img->height;
	v->coord.x = v->min_x - UV_PADDING;
	while (v->coord.x < v->max_x + UV_PADDING)
	{
		v->coord.y = v->min_y - UV_PADDING;
		while (v->coord.y < v->max_y + UV_PADDING)
		{
			check_and_color(t1, img, v);
			v->coord.y += 1;
		}
		v->coord.x += 1;
	}
}

void	copy_uv(t_tri *t1, t_uv	*v, t_bmp *img)
{
	v->min_x = t1->verts[0].txtr.x;
	v->min_x = fmin(v->min_x, t1->verts[1].txtr.x);
	v->min_x = fmin(v->min_x, t1->verts[2].txtr.x);
	if (t1->isquad)
		v->min_x = fmin(v->min_x, t1->verts[3].txtr.x);
	v->max_x = t1->verts[0].txtr.x;
	v->max_x = fmax(v->max_x, t1->verts[1].txtr.x);
	v->max_x = fmax(v->max_x, t1->verts[2].txtr.x);
	if (t1->isquad)
		v->max_x = fmax(v->max_x, t1->verts[3].txtr.x);
	v->min_y = t1->verts[0].txtr.y;
	v->min_y = fmin(v->min_y, t1->verts[1].txtr.y);
	v->min_y = fmin(v->min_y, t1->verts[2].txtr.y);
	if (t1->isquad)
		v->min_y = fmin(v->min_y, t1->verts[3].txtr.y);
	v->max_y = t1->verts[0].txtr.y;
	v->max_y = fmax(v->max_y, t1->verts[1].txtr.y);
	v->max_y = fmax(v->max_y, t1->verts[2].txtr.y);
	copy_uv2(t1, v, img);
}

void	move_uv_y(t_tri *t1, t_level *l, t_uv *v, int i)
{
	if (t1->isquad)
		t1->verts[3].txtr.y -= v->max - v->min;
	v->diff.y -= v->max - v->min;
	while (tri_uv_intersect(*t1, l->all.tris[i]))
	{
		t1->verts[0].txtr.y -= 0.01;
		t1->verts[1].txtr.y -= 0.01;
		t1->verts[2].txtr.y -= 0.01;
		if (t1->isquad)
			t1->verts[3].txtr.y -= 0.01;
		v->diff.y -= 0.01;
	}
}

void	clear_intersection(t_tri *t1, t_level *l, t_uv	*v, int i)
{
	v->max = t1->verts[0].txtr.y;
	v->max = fmax(v->max, t1->verts[1].txtr.y);
	v->max = fmax(v->max, t1->verts[2].txtr.y);
	if (t1->isquad)
		v->max = fmax(v->max, t1->verts[3].txtr.y);
	v->min = t1->verts[0].txtr.y;
	v->min = fmin(v->min, t1->verts[1].txtr.y);
	v->min = fmin(v->min, t1->verts[2].txtr.y);
	if (t1->isquad)
		v->min = fmin(v->min, t1->verts[3].txtr.y);
	t1->verts[0].txtr.y -= v->max - v->min;
	t1->verts[1].txtr.y -= v->max - v->min;
	t1->verts[2].txtr.y -= v->max - v->min;
	move_uv_y(t1, l, v, i);
}

void	baked_state(t_tri *t1, t_level *l, t_uv	*v)
{
	if (!l->spray_overlay)
		ft_error("memory allocation failed");
	l->baked = (t_color *)ft_realloc(l->baked,
			sizeof(t_color) * (l->texture.width * l->texture.height),
			sizeof(t_color)
			* (2 * l->texture.width * l->texture.height));
	if (!l->baked)
		ft_error("memory allocation failed");
	l->texture.height *= 2;
	l->normal_map.height *= 2;
	div_every_uv(l);
	v->diff.y /= 2.0;
	if (v->diff.y != 0.0 || v->diff.x != 0.0)
	{
		copy_uv(t1, v, &l->texture);
		copy_uv(t1, v, &l->normal_map);
	}
}

void	loop_state(t_tri *t1, t_level *l, t_uv	*v)
{
	l->texture.image = (int *)ft_realloc(l->texture.image,
			sizeof(int) * (l->texture.width * l->texture.height),
			sizeof(int) * (2 * l->texture.width * l->texture.height));
	if (!l->texture.image)
		ft_error("memory allocation failed");
	l->normal_map.image = (int *)ft_realloc(l->normal_map.image,
			sizeof(int) * (l->normal_map.width * l->normal_map.height),
			sizeof(int)
			* (2 * l->normal_map.width * l->normal_map.height));
	if (!l->normal_map.image)
		ft_error("memory allocation failed");
	l->spray_overlay = (unsigned *)ft_realloc(l->spray_overlay,
			sizeof(unsigned) * (l->texture.width * l->texture.height),
			sizeof(unsigned)
			* (2 * l->texture.width * l->texture.height));
	baked_state(t1, l, v);
}

void	move_uv(t_tri *t1, int t1_index, t_level *l, t_uv	*v)
{
	int		i;

	i = 0;
	v->diff.y = 0;
	v->diff.x = 0;
	while (i < l->all.tri_amount)
	{
		if (i == t1_index)
		{
			i++;
			continue ;
		}
		if (tri_uv_intersect(*t1, l->all.tris[i]))
		{
			clear_intersection(t1, l, v, i);
			i = -1;
		}
		if (t1->verts[0].txtr.y < 0 || t1->verts[1].txtr.y < 0
			|| t1->verts[2].txtr.y < 0
			|| (t1->isquad && t1->verts[3].txtr.y < 0))
			loop_state(t1, l, v);
		i++;
	}
}

void	fix_uv_overlap(t_level *level)
{
	int		i;
	int		j;
	t_uv	*uv;

	i = 0;
	(uv = (t_uv *)malloc(sizeof(t_uv)));
	if (!uv)
		ft_error("Memory Allocation failed");
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			j = 0;
			while (j < level->all.tri_amount)
			{
				if (i != j
					&& tri_uv_intersect(level->all.tris[i], level->all.tris[j]))
					move_uv(&level->all.tris[i], i, level, uv);
				j++;
			}
		}
		i++;
	}
}
