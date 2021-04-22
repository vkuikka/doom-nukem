/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 18:32:46 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/18 17:14:22 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	deselect_all_faces(t_level *level)
{
	int	i;

	i = 0;
	while (i < level->all.tri_amount)
	{
		level->all.tris[i].selected = FALSE;
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
		tmp = cast_face(level->all.tris[i], vec, NULL);
		if (tmp > 0 && tmp < dist)
		{
			dist = tmp;
			hit = i;
		}
		i++;
	}
	if (dist != FLT_MAX)
		level->all.tris[hit].selected = level->all.tris[hit].selected ? 0 : 1;
	else
		deselect_all_faces(level);
}

void	select_face(t_camera *cam, t_level *level, int x, int y)
{
	t_ray	r;
	float	xm;
	float	ym;

	r.dir = cam->front;
	r.pos = cam->pos;
	ym = cam->fov_y / RES_Y * y - cam->fov_y / 2;
	xm = cam->fov_x / RES_X * x - cam->fov_x / 2;
	r.dir.x += cam->up.x * ym + cam->side.x * xm;
	r.dir.y += cam->up.y * ym + cam->side.y * xm;
	r.dir.z += cam->up.z * ym + cam->side.z * xm;
	raycast_face_selection(r, level);
}
