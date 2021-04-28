/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_overlap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 13:48:01 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/28 15:02:15 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		div_every_uv(t_level *l)
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

void		copy_uv(t_tri *t1, float diff_y, float diff_x, t_bmp *img)
{
	float		min_x;
	float		max_x;
	float		min_y;
	float		max_y;
	int			x;
	int			y;
	
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

	x = min_x;
	while (x < max_x)
	{
		y = min_y;
		while (y < max_y)
		{
			t_vec2 check;
			check.x = x / (float)img->width;
			check.y = y / (float)img->height;
			if (point_in_tri(check, t1->verts[0].txtr, t1->verts[1].txtr, t1->verts[2].txtr) ||
				point_in_tri(check, t1->verts[3].txtr, t1->verts[1].txtr, t1->verts[2].txtr))
			{
				int	og_x = x - diff_x * img->width;
				int	og_y = (img->height - y) + diff_y * img->height;
				img->image[x + (img->height - y) * img->width] = img->image[og_x + og_y * img->width];
			}

			y++;
		}
		x++;
	}
}

void		move_uv(t_tri *t1, int t1_index, t_level *l)
{
	int		i;
	float	diff_y;
	float	diff_x;

	i = 0;
	diff_y = 0;
	diff_x = 0;
	while (i < l->all.tri_amount)
	{
		if (i == t1_index)
		{
			i++;
			continue;
		}
		if (tri_uv_intersect(*t1, l->all.tris[i]))
		{
			while (tri_uv_intersect(*t1, l->all.tris[i]))
			{
				t1->verts[0].txtr.y -= 0.01;
				t1->verts[1].txtr.y -= 0.01;
				t1->verts[2].txtr.y -= 0.01;
				if (t1->isquad)
					t1->verts[3].txtr.y -= 0.01;
				diff_y -= 0.01;
			}
			i = -1;
		}

		if (t1->verts[0].txtr.y < 0 ||
			t1->verts[1].txtr.y < 0 ||
			t1->verts[2].txtr.y < 0 ||
			(t1->isquad && t1->verts[3].txtr.y < 0))
		{
			l->texture.image = realloc(l->texture.image, sizeof(int) * (l->texture.width * (l->texture.height * 2)));
			if (!l->texture.image)
				ft_error("failed realloc\n");
			ft_bzero(&l->texture.image[l->texture.width * l->texture.height], sizeof(int) * (l->texture.width * l->texture.height));
			div_every_uv(l);
			l->texture.height *= 2;
			diff_y /= 2.0;
			printf("new height %d\n", l->texture.height);
		}

		if (l->texture.height > 10000)
			return ;
		i++;
	}
	if (diff_y != 0.0 || diff_x != 0.0)
		copy_uv(t1, diff_y, diff_x, &l->texture);
}

void			fix_uv_overlap(t_level *level)
{
	int		i;
	int		j;

	i = 0;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			j = 0;
			while (j < level->all.tri_amount)
			{
				if (i != j && tri_uv_intersect(level->all.tris[i], level->all.tris[j]))
					move_uv(&level->all.tris[i], i, level);
				j++;
			}
		}
		i++;
	}

}
