/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 17:08:49 by vkuikka           #+#    #+#             */
/*   Updated: 2021/03/24 04:06:47 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void		calc_vectors(t_tri *tri)
{
	tri->v0v1.x = tri->verts[2].pos.x - tri->verts[0].pos.x;
	tri->v0v1.y = tri->verts[2].pos.y - tri->verts[0].pos.y;
	tri->v0v1.z = tri->verts[2].pos.z - tri->verts[0].pos.z;
	tri->v0v2.x = tri->verts[1].pos.x - tri->verts[0].pos.x;
	tri->v0v2.y = tri->verts[1].pos.y - tri->verts[0].pos.y;
	tri->v0v2.z = tri->verts[1].pos.z - tri->verts[0].pos.z;
}

static float	find_angle(t_vec3 v1, t_vec3 v2)
{
	float	angle;

	angle = M_PI - vec_angle(v1, v2);
	if (isnan(angle))
		return (0);
	rotate_vertex(M_PI / 2, &v2, 0);
	angle *= vec_dot(v1, v2) < 0 ? -1 : 1;
	return (angle);
}

static void		turn_sprite(t_tri *tri, t_vec3 dir)
{
	t_vec3	face_mid;
	t_vec3	rot_vert;
	t_vec3	normal;
	float	angle;
	int		vert;

	vert = 0;
	vec_cross(&normal, tri->v0v1, tri->v0v2);
	normal.y = 0;
	ft_memset(&face_mid, 0, sizeof(int) * 3);
	while (vert < 3 + tri->isquad)
		vec_add(&face_mid, face_mid, tri->verts[vert++].pos);
	vec_div(&face_mid, (float)vert);
	vec_sub(&rot_vert, dir, face_mid);
	rot_vert.y = 0;
	if (!(angle = find_angle(normal, rot_vert)))
		return;
	while (vert--)
	{
		vec_sub(&rot_vert, tri->verts[vert].pos, face_mid);
		rotate_vertex(angle, &rot_vert, 0);
		vec_add(&tri->verts[vert].pos, rot_vert, face_mid);
	}
	calc_vectors(tri);
}

void			enemies(t_level *level)
{
	int		face;

	face = 0;
	global_seginfo = "enemies\n";
	while (face < level->obj[0].tri_amount)
	{
		if (level->obj[0].tris[face].isenemy)
		{
			turn_sprite(&level->obj[0].tris[face], level->pos);
			// add shooting and movement functions here
		}
		face++;
	}
}
