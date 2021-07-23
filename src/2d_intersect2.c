/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2d_intersect2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsjoberg <lsjoberg@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/23 15:46:09 by lsjoberg          #+#    #+#             */
/*   Updated: 2021/07/23 16:55:24 by lsjoberg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static float	sign(t_vec2 p1, t_vec2 p2, t_vec2 p3)
{
	return ((p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y));
}

int	point_in_tri(t_vec2 pt, t_vec2 v1, t_vec2 v2, t_vec2 v3)
{
	float	d1;
	float	d2;
	float	d3;
	int		has_neg;
	int		has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);
	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
	return (!(has_neg && has_pos));
}

/*
**	Given three colinear points q, p, r, the function checks if point q lies 
**	on line segment 'pr'
*/

static int	on_segment(t_vec2 q, t_vec2 p, t_vec2 r)
{
	if (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x)
		&& q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y))
		return (TRUE);
	return (FALSE);
}
/*
**	https://www.geeksforgeeks.org/orientation-3-ordered-points/
**	return 0 is colinear (point is on the line (not segment)
**	return 1 is clockwise
**	return 2 is counter clockwise
*/

int	line_orientation(t_vec2 p, t_vec2 q, t_vec2 r)
{
	float	val;

	val = (q.y - p.y) * (r.x - q.x)
		- (q.x - p.x) * (r.y - q.y);
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

int	line_intersect(t_vec2 p1, t_vec2 q1, t_vec2 p2, t_vec2 q2)
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
