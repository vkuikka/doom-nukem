/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_overlap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 13:48:01 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/12 11:38:22 by rpehkone         ###   ########.fr       */
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
static void	check_and_color(t_tri *t1, t_bmp *img, t_ivec2 coord, t_vec2 diff)
{
	t_vec2	precision;
	t_vec2	coord_uv;
	t_vec2	check;
	int		og_x;
	int		og_y;
	int		x;
	int		y;

	precision.x = 1 / (float)img->width;
	precision.y = 1 / (float)img->height;
	coord_uv.x = coord.x / (float)img->width - precision.x;
	coord_uv.y = coord.y / (float)img->height - precision.y;
	x = -1;
	while (++x < 3)
	{
		y = -1;
		while (++y < 3)
		{
			check.x = coord_uv.x + precision.x * x;
			check.y = coord_uv.y + precision.y * y;
			if (point_in_tri(check,
					t1->verts[0].txtr, t1->verts[1].txtr, t1->verts[2].txtr)
				|| (t1->isquad && point_in_tri(check, t1->verts[3].txtr,
						t1->verts[1].txtr, t1->verts[2].txtr)))
			{
				og_x = coord.x - diff.x * img->width;
				og_y = (img->height - coord.y) + diff.y * img->height;
				img->image[coord.x + (img->height - coord.y) * img->width]
					= img->image[og_x + og_y * img->width];
				return ;
			}
		}
	}
}

void	copy_uv(t_tri *t1, t_vec2 diff, t_bmp *img)
{
	float	min_x;
	float	max_x;
	float	min_y;
	float	max_y;
	t_ivec2	coord;

	min_x = t1->verts[0].txtr.x;
	min_x = fmin(min_x, t1->verts[1].txtr.x);
	min_x = fmin(min_x, t1->verts[2].txtr.x);
	if (t1->isquad)
		min_x = fmin(min_x, t1->verts[3].txtr.x);
	max_x = t1->verts[0].txtr.x;
	max_x = fmax(max_x, t1->verts[1].txtr.x);
	max_x = fmax(max_x, t1->verts[2].txtr.x);
	if (t1->isquad)
		max_x = fmax(max_x, t1->verts[3].txtr.x);
	min_y = t1->verts[0].txtr.y;
	min_y = fmin(min_y, t1->verts[1].txtr.y);
	min_y = fmin(min_y, t1->verts[2].txtr.y);
	if (t1->isquad)
		min_y = fmin(min_y, t1->verts[3].txtr.y);
	max_y = t1->verts[0].txtr.y;
	max_y = fmax(max_y, t1->verts[1].txtr.y);
	max_y = fmax(max_y, t1->verts[2].txtr.y);
	if (t1->isquad)
		max_y = fmax(max_y, t1->verts[3].txtr.y);
	min_x *= (float)img->width;
	max_x *= (float)img->width;
	max_y *= (float)img->height;
	min_y *= (float)img->height;
	coord.x = min_x - UV_PADDING;
	while (coord.x < max_x + UV_PADDING)
	{
		coord.y = min_y - UV_PADDING;
		while (coord.y < max_y + UV_PADDING)
		{
			check_and_color(t1, img, coord, diff);
			coord.y += 1;
		}
		coord.x += 1;
	}
}

void	clear_intersection(t_tri *t1, t_level *l, t_vec2 *diff, int i)
{
	float	max;
	float	min;

	max = t1->verts[0].txtr.y;
	max = fmax(max, t1->verts[1].txtr.y);
	max = fmax(max, t1->verts[2].txtr.y);
	if (t1->isquad)
		max = fmax(max, t1->verts[3].txtr.y);
	min = t1->verts[0].txtr.y;
	min = fmin(min, t1->verts[1].txtr.y);
	min = fmin(min, t1->verts[2].txtr.y);
	if (t1->isquad)
		min = fmin(min, t1->verts[3].txtr.y);
	t1->verts[0].txtr.y -= max - min;
	t1->verts[1].txtr.y -= max - min;
	t1->verts[2].txtr.y -= max - min;
	if (t1->isquad)
		t1->verts[3].txtr.y -= max - min;
	diff->y -= max - min;
	while (tri_uv_intersect(*t1, l->all.tris[i]))
	{
		t1->verts[0].txtr.y -= 0.01;
		t1->verts[1].txtr.y -= 0.01;
		t1->verts[2].txtr.y -= 0.01;
		if (t1->isquad)
			t1->verts[3].txtr.y -= 0.01;
		diff->y -= 0.01;
	}
}

void	move_uv(t_tri *t1, int t1_index, t_level *l)
{
	int		i;
	t_vec2	diff;

	i = 0;
	diff.y = 0;
	diff.x = 0;
	while (i < l->all.tri_amount)
	{
		if (i == t1_index)
		{
			i++;
			continue ;
		}
		if (tri_uv_intersect(*t1, l->all.tris[i]))
		{
			clear_intersection(t1, l, &diff, i);
			i = -1;
		}
		if (t1->verts[0].txtr.y < 0 || t1->verts[1].txtr.y < 0
			|| t1->verts[2].txtr.y < 0
			|| (t1->isquad && t1->verts[3].txtr.y < 0))
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
			diff.y /= 2.0;
		}
		i++;
	}
	if (diff.y != 0.0 || diff.x != 0.0)
	{
		copy_uv(t1, diff, &l->texture);
		copy_uv(t1, diff, &l->normal_map);
	}
}

void	fix_uv_overlap(t_level *level)
{
	int	i;
	int	j;

	i = 0;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			j = 0;
			while (j < level->all.tri_amount)
			{
				if (i != j
					&& tri_uv_intersect(level->all.tris[i], level->all.tris[j]))
					move_uv(&level->all.tris[i], i, level);
				j++;
			}
		}
		i++;
	}
}
