/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2d_intersect.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsjoberg <lsjoberg@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 15:15:59 by vkuikka           #+#    #+#             */
/*   Updated: 2021/07/23 16:55:38 by lsjoberg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	calc_quad_uv(t_tri *tri)
{
	tri->verts[3].txtr.x = tri->verts[2].txtr.x
		+ (tri->verts[1].txtr.x - tri->verts[0].txtr.x);
	tri->verts[3].txtr.y = tri->verts[2].txtr.y
		+ (tri->verts[1].txtr.y - tri->verts[0].txtr.y);
}

/*
**	Returns true if two faces overlap in texture coordinates
*/

int	intersect1(t_tri t1, t_tri t2, int i, int j)
{
	if (point_in_tri(t1.verts[i].txtr, t2.verts[0].txtr, t2.verts[1].txtr,
			t2.verts[2].txtr) || (t2.isquad
			&& point_in_tri(t1.verts[i].txtr,
				t2.verts[3].txtr, t2.verts[1].txtr, t2.verts[2].txtr)))
		return (1);
	return (0);
}

int	intersect2(t_tri t1, t_tri t2, int k, int l)
{
	if (l == 2 + t2.isquad)
	{
		if (line_intersect(t1.verts[k].txtr, t1.verts[0].txtr,
				t2.verts[l].txtr, t2.verts[0].txtr))
			return (1);
	}
	else
	{
		if (line_intersect(t1.verts[k].txtr, t1.verts[0].txtr,
				t2.verts[l].txtr, t2.verts[l + 1].txtr))
			return (1);
	}
	return (0);
}

int	intersect3(t_tri t1, t_tri t2, int k, int l)
{
	if (l == 2 + t2.isquad)
	{
		if (line_intersect(t1.verts[k].txtr, t1.verts[k + 1].txtr,
				t2.verts[l].txtr, t2.verts[0].txtr))
			return (1);
	}
	else
	{
		if (line_intersect(t1.verts[k].txtr, t1.verts[l + 1].txtr,
				t2.verts[l].txtr, t2.verts[l + 1].txtr))
			return (1);
	}
	return (0);
}

int	tri_uv_intersect(t_tri t1, t_tri t2)
{
	int	i;
	int	j;

	i = 0;
	if (t1.isquad)
		calc_quad_uv(&t1);
	if (t2.isquad)
		calc_quad_uv(&t2);
	while (i < 3 + t1.isquad)
	{
		intersect1(t1, t2, i, j);
		j = 0;
		while (j < 3 + t2.isquad)
		{
			if (i == 2 + t1.isquad)
				intersect2(t1, t2, i, j);
			else
				intersect3(t1, t2, i, j);
			j++;
		}
		i++;
	}
	return (0);
}
