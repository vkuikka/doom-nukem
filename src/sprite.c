/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_0.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

float	find_angle(t_vec3 v1, t_vec3 v2)
{
	float	angle;

	angle = M_PI - vec_angle(v1, v2);
	if (isnan(angle))
		return (0);
	rotate_vertex(M_PI / 2, &v2, 0);
	if (vec_dot(v1, v2) < 0)
		angle *= -1;
	return (angle);
}

void	turn_sprite(t_tri *tri, t_vec3 look_at)
{
	t_vec3	face_mid;
	t_vec3	rot_vert;
	t_vec3	normal;
	float	angle;
	int		vert;

	normal = tri->normal;
	normal.y = 0;
	face_mid = (t_vec3){0, 0, 0};
	vert = 0;
	while (vert < 3 + tri->isquad)
		vec_add(&face_mid, face_mid, tri->verts[vert++].pos);
	vec_div(&face_mid, (float)vert);
	vec_sub(&rot_vert, look_at, face_mid);
	rot_vert.y = 0;
	angle = find_angle(normal, rot_vert);
	if (!angle)
		return ;
	while (vert--)
	{
		vec_sub(&rot_vert, tri->verts[vert].pos, face_mid);
		rotate_vertex(angle, &rot_vert, 0);
		vec_add(&tri->verts[vert].pos, rot_vert, face_mid);
	}
}

void	merge_sprite(t_level *level, t_vec3 pos, t_bmp *texture)
{
	int	i;

	visible_request_merge(level, 1);
	i = level->visible.tri_amount;
	set_new_face(&level->visible.tris[i], pos, (t_vec3){0, 0, 0}, .1);
	level->visible.tris[i].isquad = TRUE;
	turn_sprite(&level->visible.tris[i], level->cam.pos);
	tri_optimize(&level->visible.tris[i]);
	level->visible.tris[i].texture = texture;
	level->visible.tri_amount++;
}
