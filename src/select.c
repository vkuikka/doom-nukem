/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 18:32:46 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/08 17:52:34 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	deselect_all_faces(t_level *level)
{
	int	i;
	int	j;

	i = 0;
	while (i < level->all.tri_amount)
	{
		j = 0;
		while (j < 3 + level->all.tris[i].isquad)
		{
			level->all.tris[i].verts[j].selected = 0;
			j++;
		}
		i++;
	}
}

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
	else
		deselect_all_faces(level);
}

void	select_face(t_level *level, int x, int y)
{
	t_ray	r;
	t_vec3	up;
	t_vec3	side;
	float	xm;
	float	ym;

	xm = -level->look_side + M_PI/2;
	ym = -level->look_up;
	rot_cam(&r.dir, xm, ym);
	vec_copy(&r.pos, level->pos);
	rot_cam(&up, xm, ym + (M_PI / 2));
	vec_cross(&side, up, r.dir);
	ym = level->ui->fov / RES_Y * y - level->ui->fov / 2;
	xm = (level->ui->fov * ((float)RES_X / RES_Y)) / RES_X * x - (level->ui->fov * ((float)RES_X / RES_Y) / 2);
	r.dir.x += up.x * ym + side.x * xm;
	r.dir.y += up.y * ym + side.y * xm;
	r.dir.z += up.z * ym + side.z * xm;
	raycast_face_selection(r, level);
}
