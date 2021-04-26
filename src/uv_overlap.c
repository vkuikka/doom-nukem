/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_overlap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 13:48:01 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/26 20:05:16 by vkuikka          ###   ########.fr       */
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

void		copy_uv(t_tri *t1, t_tri *t2, t_level *l)
{
	int		i;

	while (tri_uv_intersect(*t1, *t2))
	{
		t1->verts[0].txtr.y -= 0.01;
		t1->verts[1].txtr.y -= 0.01;
		t1->verts[2].txtr.y -= 0.01;
		if (t1->isquad)
			t1->verts[3].txtr.y -= 0.01;
	}
	if (l->texture.height > 10000)
		return ;

	i = 0;
	while (i < l->all.tri_amount)
	{
		if (t1->selected)
		{
			if (t1->verts[0].txtr.y < 0 ||
				t1->verts[1].txtr.y < 0 ||
				t1->verts[2].txtr.y < 0)
			{
				l->texture.image = realloc(l->texture.image, sizeof(int) * (l->texture.width * (l->texture.height * 2)));
				if (!l->texture.image)
					ft_error("failed realloc\n");
				ft_bzero(&l->texture.image[l->texture.width * l->texture.height], sizeof(int) * (l->texture.width * l->texture.height));
				div_every_uv(l);
				l->texture.height *= 2;
				printf("new height %d\n", l->texture.height);
			}
		}
		i++;
	}
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
				{
					copy_uv(&level->all.tris[i], &level->all.tris[j], level);
					// level->all.tris[i].selected = 1;
					// level->all.tris[j].selected = 1;
				}
				j++;
			}
		}
		i++;
	}

}
