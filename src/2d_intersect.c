/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2d_intersect.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcohen <alcohen@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 15:15:59 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/28 23:03:51 by alcohen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static float	sign(t_vec2 p1, t_vec2 p2, t_vec2 p3)
{
	return ((p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y));
}

int	point_in_tri(t_vec2 pt, t_vec2 v1, t_vec2 v2, t_vec2 v3)
{
	int		has_neg;
	int		has_pos;
	float	d1;
	float	d2;
	float	d3;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);
	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
	return (!(has_neg && has_pos));
}

/*
** Given three colinear points q, p, r, the function checks if point q lies on
* line segment 'pr'
*/
//unused what going on
// static int	on_segment(t_vec2 q, t_vec2 p, t_vec2 r)
// {
// 	if (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x)
// 		&& q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y))
// 		return (TRUE);
// 	return (FALSE);
// }

// https://www.geeksforgeeks.org/orientation-3-ordered-points/
// Returns 0 (colinear, point is on the line (not segment)),
// 		   1 (clockwise) or 2 (counter-clockwise)
static int	line_orientation(t_vec2 p, t_vec2 q, t_vec2 r)
{
	float	val;

	val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val == 0)
		return (0);
	if (val > 0)
		return (1);
	else
		return (2);
}

/*
** Returns true if line segments 'p1q1' and 'p2q2' intersect.
** Does not check if point is exactly on segment
*/
static int	line_intersect(t_vec2 p1, t_vec2 q1, t_vec2 p2, t_vec2 q2)
{
	int	o1;
	int	o2;
	int	o3;
	int	o4;

	o1 = line_orientation(p1, q1, p2);
	o2 = line_orientation(p1, q1, q2);
	o3 = line_orientation(p2, q2, p1);
	o4 = line_orientation(p2, q2, q1);
	if (o1 != o2 && o3 != o4)
		return (TRUE);
	return (FALSE);
}

static void	calc_quad_uv(t_tri *tri)
{
	tri->verts[3].txtr.x
		= tri->verts[2].txtr.x + (tri->verts[1].txtr.x - tri->verts[0].txtr.x);
	tri->verts[3].txtr.y
		= tri->verts[2].txtr.y + (tri->verts[1].txtr.y - tri->verts[0].txtr.y);
}

/*
** Returns true if two faces overlap in texture coordinates
*/
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
		if (point_in_tri(t1.verts[i].txtr, t2.verts[0].txtr,
				t2.verts[1].txtr, t2.verts[2].txtr) || (t2.isquad
				&& point_in_tri(t1.verts[i].txtr, t2.verts[3].txtr,
					t2.verts[1].txtr, t2.verts[2].txtr)))
			return (1);
		j = 0;
		while (j < 3 + t2.isquad)
		{
			if (i == 2 + t1.isquad)
			{
				if (j == 2 + t2.isquad)
				{
					if (line_intersect(t1.verts[i].txtr, t1.verts[0].txtr,
							t2.verts[j].txtr, t2.verts[0].txtr))
						return (1);
				}
				else
				{
					if (line_intersect(t1.verts[i].txtr, t1.verts[0].txtr,
							t2.verts[j].txtr, t2.verts[j + 1].txtr))
						return (1);
				}
			}
			else
			{
				if (j == 2 + t2.isquad)
				{
					if (line_intersect(t1.verts[i].txtr, t1.verts[i + 1].txtr,
							t2.verts[j].txtr, t2.verts[0].txtr))
						return (1);
				}
				else
				{
					if (line_intersect(t1.verts[i].txtr, t1.verts[i + 1].txtr,
							t2.verts[j].txtr, t2.verts[j + 1].txtr))
						return (1);
				}
			}
			j++;
		}
		i++;
	}
	return (0);
}
