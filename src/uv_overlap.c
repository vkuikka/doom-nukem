/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_overlap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 13:48:01 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/26 15:53:24 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static float	uv_min(t_tri tri)
{
	float	min;

	min = tri.verts[0].txtr.y;
	if (tri.verts[1].txtr.y < min)
		min = tri.verts[1].txtr.y;
	if (tri.verts[2].txtr.y < min)
		min = tri.verts[2].txtr.y;
	return (min);
}

static float	uv_max(t_tri tri)
{
	float		max;

	max = tri.verts[0].txtr.y;
	if (tri.verts[1].txtr.y > max)
		max = tri.verts[1].txtr.y;
	if (tri.verts[2].txtr.y > max)
		max = tri.verts[2].txtr.y;
	return (max);
}

void		copy_uv(t_tri *t1, t_tri *t2, t_level *l)
{
	float	height;

	while (tri_uv_intersect(*t1, *t2))
	{
		t2->verts[0].txtr.y -= 0.01;
		t2->verts[1].txtr.y -= 0.01;
		t2->verts[2].txtr.y -= 0.01;
		// if (t2->verts[0].txtr.y > l->texture.height ||
		// 	t2->verts[1].txtr.y > l->texture.height ||
		// 	t2->verts[2].txtr.y > l->texture.height)
		// {
		// 	height = uv_max(*t2) - uv_min(*t2);
		// 	l->texture.image = realloc(l->texture.image, l->texture.width * (l->texture.height * 2));
		// 	if (!l->texture.image)
		// 		ft_error("failed realloc\n");
		// 	ft_memcpy(&l->texture.image[l->texture.width * l->texture.height], l->texture.image, l->texture.width * l->texture.height);
		// 	l->texture.height *= 2;
		// 	printf("%d\n", l->texture.height);
		// }
	}
	// exit(1);
}

void			fix_uv_overlap(t_level *level)
{
	int		i;
	int		j;

	i = 0;
	while (i < level->all.tri_amount)
	{
		j = 0;
		while (j < level->all.tri_amount)
		{
			if (i != j && tri_uv_intersect(level->all.tris[i], level->all.tris[j]))
			{
				// printf("%d %d\n", i, j);
				level->all.tris[i].selected = 1;
				level->all.tris[j].selected = 1;
				copy_uv(&level->all.tris[i], &level->all.tris[j], level);
			}
			// else
			// 	level->all.tris[i].selected = 0;
			// 	level->all.tris[j].selected = 0;
			j++;
		}
		i++;
	}
	if (level->texture.height > 4000)
		return ;
	i = 0;
	t_level *l = level;
	float height;
	while (i < level->all.tri_amount)
	{
		t_tri *t2 = &level->all.tris[i];
		if (t2->selected)
		{
			// printf("%f %f %f\n", t2->verts[0].txtr.y);
			// printf("%f %f %f\n", t2->verts[0].txtr.y);
			// printf("%f %f %f\n", t2->verts[0].txtr.y);
			// if (t2->verts[0].txtr.y > l->texture.height ||
			// 	t2->verts[1].txtr.y > l->texture.height ||
			// 	t2->verts[2].txtr.y > l->texture.height)
			if (t2->verts[0].txtr.y < 0 ||
				t2->verts[1].txtr.y < 0 ||
				t2->verts[2].txtr.y < 0)
			{
				height = uv_max(*t2) - uv_min(*t2);
				l->texture.image = realloc(l->texture.image, sizeof(int) * (l->texture.width * (l->texture.height * 2)));
				// if (!l->texture.image)
				// 	ft_error("failed realloc\n");
				ft_memcpy(&l->texture.image[l->texture.width * l->texture.height], l->texture.image, sizeof(int) * (l->texture.width * l->texture.height));
				l->texture.height *= 2;
				printf("asd %d\n", l->texture.height);
			}
		}
		i++;
	}
	printf("%d\n", level->texture.height);
}
