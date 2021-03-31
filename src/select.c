/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 18:32:46 by vkuikka           #+#    #+#             */
/*   Updated: 2021/03/31 19:03:17 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	raycast_face_selection(t_ray vec, t_level *level)
{
	float	dist;
	float	tmp;
	int		hit;
	int		i;

	i = 0;
	dist = FLT_MAX;
	while (i < level->all.tri_amount)
	{
		tmp = cast_face(level->all.tris[i], vec, NULL, NULL);
		if (tmp > 0 && tmp < dist)
		{
			dist = tmp;
			hit = i;
		}
		i++;
	}
	if (dist != FLT_MAX)
	{
		i = 0;
		while (i < 3 + level->all.tris[hit].isquad)
		{
			if (level->all.tris[hit].verts[i].selected)
				level->all.tris[hit].verts[i].selected = 0;
			else
				level->all.tris[hit].verts[i].selected = 1;
			i++;
		}
	}
}

void	select_face(t_level *level, int x, int y)
{
	t_vec3	rot;
	t_ray	r;
	float	lon = -level->look_side + M_PI/2;
	float	lat = -level->look_up;
	float	ym;

	ym = 1.0 / RES_Y * y - 0.5;
	r.pos.x = level->pos.x;
	r.pos.y = level->pos.y;
	r.pos.z = level->pos.z;
	rot_cam(&r.dir, lon, lat);
	rot_cam(&rot, lon, lat + (M_PI / 2));
	r.dir.x += rot.x * ym;
	r.dir.y += rot.y * ym;
	r.dir.z += rot.z * ym;
	vec_cross(&rot, rot, r.dir);
	vec_mult(&rot, (1.0 / RES_X * x - 0.5));
	vec_add(&r.dir, r.dir, rot);
	raycast_face_selection(r, level);
}
