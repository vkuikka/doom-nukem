/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2d_intersect.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/26 15:15:59 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/26 15:17:36 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

// static float	sign(t_vec2 p1, t_vec2 p2, t_vec2 p3)
// {
//     return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
// }

// static int		point_in_tri(t_vec2 pt, t_tri tri)
// {
//     float	d1, d2, d3;
//     int		has_neg, has_pos;

//     d1 = sign(pt, tri.verts[0].pos, tri.verts[1].pos);
//     d2 = sign(pt, tri.verts[1].pos, tri.verts[2].pos);
//     d3 = sign(pt, tri.verts[2].pos, tri.verts[0].pos);
//     has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
//     has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
//     return !(has_neg && has_pos);
// }

/*
**	Given three colinear points q, p, r, the function checks if point q lies on line segment 'pr'
*/
static int		on_segment(t_vec2 q, t_vec2 p, t_vec2 r)
{
    if (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x) &&
        q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y))
		return (TRUE);
	return (FALSE);
}

static int		line_orientation(t_vec2 p, t_vec2 q, t_vec2 r)
{
	// https://www.geeksforgeeks.org/orientation-3-ordered-points/
	float	val;
	val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);
	// printf("%f\n", val);
	// exit(1);

	if (val == 0)
		return (0);	// colinear	(point is on the line (not segment) this is prob useless)
	if (val > 0)
		return (1);	//clockwise
	else
		return (2);	//counter clockwise

	// return ((val > 0)? 1: 2); // clock or counterclock wise
}
  
/*
** Returns true if line segments 'p1q1' and 'p2q2' intersect.
*/
static int		line_intersect(t_vec2 p1, t_vec2 q1, t_vec2 p2, t_vec2 q2)
{
    // Find the four orientations needed
    int o1 = line_orientation(p1, q1, p2);
    int o2 = line_orientation(p1, q1, q2);
    int o3 = line_orientation(p2, q2, p1);
    int o4 = line_orientation(p2, q2, q1);
  
    // General case (lines intersect)

	// printf("%d %d %d %d\n", o1, o2, o3, o4);
	// printf("%f %f, %f %f\n", p1.x, p1.y, q1.x, q1.y);
	// printf("%f %f, %f %f\n", p2.x, p2.y, q2.x, q2.y);
	// printf("\n");
    if (o1 != o2 && o3 != o4)
	{
		if (o1 && o2 && o3 && o4)
		{
			// printf("%d %d %d %d\n", o1, o2, o3, o4);
			// printf("%f %f, %f %f\n", p1.x, p1.y, q1.x, q1.y);
			// printf("%f %f, %f %f\n", p2.x, p2.y, q2.x, q2.y);
			// printf("\n");
			// exit(1);
			return (TRUE);
		}
	}
	return (FALSE);	//	on segment does not matter
  
    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && on_segment(p1, p2, q1))
		return (TRUE);
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && on_segment(p1, q2, q1))
		return (TRUE);
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && on_segment(p2, p1, q2))
		return (TRUE);
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && on_segment(p2, q1, q2))
		return (TRUE);
  
    return (FALSE); // Doesn't fall in any of the above cases
}

/*
**	Returns true if two faces overlap in texture coordinates
*/
int				tri_uv_intersect(t_tri t1, t_tri t2)
{
	int		i;
	int		j;

	i = 0;
	// printf("quad %d %d ", t1.isquad, t2.isquad);
	t1.isquad = 0;
	t2.isquad = 0;
	// t1.verts[0].txtr.x = 0.0;
	// t1.verts[0].txtr.y = 0.5;
	// t1.verts[1].txtr.x = 0.5;
	// t1.verts[1].txtr.y = 0.5;

	// t2.verts[0].txtr.x = 0.5;
	// t2.verts[0].txtr.y = 1.0;
	// t2.verts[1].txtr.x = 0.5;
	// t2.verts[1].txtr.y = 0.0;

	while (i < 3 + t1.isquad)
	{
		j = 0;
		while (j < 3 + t2.isquad)
		{
			// printf("%d, ", i);
			// printf("%d\n", j);
			if (i == 2 + t2.isquad)
			{
				if (j == 2 + t2.isquad)	// both last index
				{
					// printf("both last\n");
					if (line_intersect(t1.verts[i].txtr, t1.verts[0].txtr, t2.verts[j].txtr, t2.verts[0].txtr))
						return (1);
				}
				else					// i last index
				{
					// printf("i last\n");
					if (line_intersect(t1.verts[i].txtr, t1.verts[0].txtr, t2.verts[j].txtr, t2.verts[j + 1].txtr))
						return (1);
				}
			}
			else
			{
				if (j == 2 + t2.isquad)	// j last index
				{
					// printf("j last\n");
					if (line_intersect(t1.verts[i].txtr, t1.verts[i + 1].txtr, t2.verts[j].txtr, t2.verts[0].txtr))
						return (1);
				}
				else					// not last index
				{
					// printf("not last\n");
					if (line_intersect(t1.verts[i].txtr, t1.verts[i + 1].txtr, t2.verts[j].txtr, t2.verts[j + 1].txtr))
					{
						// exit(1);
						return (1);
					}
						// exit(1);
				}

			}
			j++;
		}
		i++;
	}
	// printf("done\n");
	// exit(1);
	return (0);
}
