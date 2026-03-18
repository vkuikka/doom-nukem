/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 15:02:42 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

int	cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri)
{
	t_vec3	vert;
	int		i;

	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&vert, tri.verts[i].pos, pos);
		if (vec_dot(dir, vert) < 0)
			return (TRUE);
		i++;
	}
	return (FALSE);
}

int	cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri)
{
	t_vec3	vert;
	int		i;

	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&vert, tri.verts[i].pos, pos);
		if (vec_dot(dir, vert) > 0)
			return (TRUE);
		i++;
	}
	return (FALSE);
}

int	frustrum_culling(t_vec3 side_normal[4], t_vec3 pos, t_tri tri)
{
	t_vec3	end;
	int		out[4][4];
	int		i;

	out[3][0] = 1;
	out[3][1] = 1;
	out[3][2] = 1;
	out[3][3] = 1;
	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&end, tri.verts[i].pos, pos);
		out[i][0] = vec_dot(side_normal[0], end) < 0;
		out[i][1] = vec_dot(side_normal[1], end) < 0;
		out[i][2] = vec_dot(side_normal[2], end) < 0;
		out[i][3] = vec_dot(side_normal[3], end) < 0;
		if (!out[i][0] && !out[i][1] && !out[i][2] && !out[i][3])
			return (1);
		i++;
	}
	i = -1;
	while (++i < 4)
		if (out[0][i] && out[1][i] && out[2][i] && out[3][i])
			return (0);
	return (1);
}

void	distance_minmax(float *min, float *max, t_tri tri, t_vec3 pos)
{
	t_vec3	diff;
	float	tmp;
	int		i;

	i = 0;
	*min = FLT_MAX;
	*max = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&diff, tri.verts[i].pos, pos);
		tmp = vec_length(diff);
		if (tmp < *min)
			*min = tmp;
		if (tmp > *max)
			*max = tmp;
		i++;
	}
}

int	distance_culling(t_tri tri, t_vec3 player, float render_distance)
{
	float	min;
	float	max;
	float	len;
	t_vec3	tmp_vec;

	distance_minmax(&min, &max, tri, player);
	if (min < render_distance)
		return (1);
	vec_sub(&tmp_vec, tri.verts[2].pos, tri.verts[1].pos);
	min = vec_length(tmp_vec);
	len = vec_length(tri.v0v1);
	if (len < min)
		len = min;
	min = vec_length(tri.v0v2);
	if (len < min)
		len = min;
	if (tri.isquad)
	{
		vec_sub(&tmp_vec, tri.verts[3].pos, tri.verts[0].pos);
		min = vec_length(tmp_vec);
		if (len < min)
			len = min;
	}
	return (max < len);
}

int	backface_culling(t_vec3 pos, t_tri tri)
{
	t_vec3	normal;
	t_vec3	diff;

	vec_cross(&normal, tri.v0v1, tri.v0v2);
	vec_sub(&diff, tri.verts[0].pos, pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(&diff, tri.verts[1].pos, pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	vec_sub(&diff, tri.verts[2].pos, pos);
	if (vec_dot(diff, normal) > 0)
		return (1);
	if (tri.isquad)
	{
		vec_sub(&diff, tri.verts[3].pos, pos);
		if (vec_dot(diff, normal) > 0)
			return (1);
	}
	return (0);
}

int	reflection_backface(t_tri t1, t_tri t2)
{
	int	i;

	i = 0;
	while (i < 3 + t2.isquad)
	{
		if (backface_culling(t2.verts[i].pos, t1))
			return (1);
		i++;
	}
	return (0);
}

void	calculate_side_normals(t_vec3 normal[4], t_vec3 corner[4])
{
	vec_cross(&normal[0], corner[2], corner[0]);
	vec_cross(&normal[1], corner[1], corner[3]);
	vec_cross(&normal[2], corner[0], corner[1]);
	vec_cross(&normal[3], corner[3], corner[2]);
}

void	face_border_normals(t_vec3 v[4], t_vec3 dir, t_tri target)
{
	t_vec3	tmp;

	vec_sub(&tmp, target.verts[0].pos, target.verts[1].pos);
	vec_cross(&v[0], tmp, dir);
	vec_sub(&tmp, target.verts[2].pos, target.verts[0].pos);
	vec_cross(&v[2], tmp, dir);
	if (target.isquad)
	{
		vec_sub(&tmp, target.verts[1].pos, target.verts[3].pos);
		vec_cross(&v[1], tmp, dir);
		vec_sub(&tmp, target.verts[3].pos, target.verts[2].pos);
		vec_cross(&v[3], tmp, dir);
	}
	else
	{
		vec_sub(&tmp, target.verts[1].pos, target.verts[2].pos);
		vec_cross(&v[1], tmp, dir);
	}
}

void	shadow_face_culling(t_level *level, int i)
{
	t_tri	target;
	t_vec3	v[4];
	int		k;

	target = level->all.tris[i];
	face_border_normals(v, level->ui.sun_dir, target);
	k = -1;
	level->all.tris[i].shadow_faces.tri_amount = 0;
	while (++k < level->all.tri_amount)
		if (k != i && cull_behind(level->all.tris[i].normal,
				target.verts[0].pos, level->all.tris[k])
			&& (level->all.tris[k].isgrid || target.isgrid
				|| (cull_behind(v[0], target.verts[0].pos, level->all.tris[k])
					&& cull_behind(v[1], target.verts[1].pos,
						level->all.tris[k])
					&& cull_behind(v[2], target.verts[2].pos,
						level->all.tris[k])
					&& (!level->all.tris[i].isquad
						|| cull_behind(v[3], target.verts[3].pos,
							level->all.tris[k])))))
			level->all.tris[i].shadow_faces.tris[
				level->all.tris[i].shadow_faces.tri_amount++]
				= level->all.tris[k];
}

void	vertex_directions(t_vec3 corner[4], t_level *l, int i, t_vec3 pos)
{
	corner[0] = l->all.tris[i].verts[0].pos;
	corner[1] = l->all.tris[i].verts[1].pos;
	corner[2] = l->all.tris[i].verts[2].pos;
	corner[3] = l->all.tris[i].verts[3].pos;
	vec_sub(&corner[0], pos, corner[0]);
	vec_sub(&corner[1], pos, corner[1]);
	vec_sub(&corner[2], pos, corner[2]);
	vec_sub(&corner[3], pos, corner[3]);
	vec_normalize(&corner[0]);
	vec_normalize(&corner[1]);
	vec_normalize(&corner[2]);
	vec_normalize(&corner[3]);
}

t_vec3	camera_to_reflection(t_level *level, int i)
{
	t_vec3	normal;
	t_vec3	pos;

	memset(&pos, 0, sizeof(t_vec3));
	level->all.tris[i].reflection_obj_first_bounce.tri_amount = 0;
	vec_sub(&pos, level->all.tris[i].verts[0].pos, level->cam.pos);
	normal = level->all.tris[i].normal;
	vec_mult(&normal, vec_dot(pos, normal));
	vec_add(&pos, level->cam.pos, normal);
	vec_add(&pos, pos, normal);
	return (pos);
}

void	reflection_culling_first_bounce(t_level *level, int i)
{
	t_vec3	pos;
	t_vec3	corner[4];
	t_vec3	side_normals[4];
	int		k;

	pos = camera_to_reflection(level, i);
	vertex_directions(corner, level, i, pos);
	calculate_side_normals(side_normals, corner);
	if (!level->all.tris[i].isquad)
		side_normals[3] = side_normals[0];
	k = -1;
	while (++k < level->all.tris[i].reflection_obj_all.tri_amount)
		if ((!level->ui.distance_culling
				|| distance_culling(
					level->all.tris[i].reflection_obj_all.tris[k],
					level->cam.pos, level->ui.render_distance))
			&& (level->all.tris[i].reflection_obj_all.tris[k].isgrid
				|| frustrum_culling(side_normals, pos,
					level->all.tris[i].reflection_obj_all.tris[k])))
			level->all.tris[i].reflection_obj_first_bounce.tris[
				level->all.tris[i].reflection_obj_first_bounce.tri_amount++]
				= level->all.tris[i].reflection_obj_all.tris[k];
}

int	face_distance_culling(t_tri t1, t_tri t2, t_level *level)
{
	int	i;

	i = 0;
	if (!level->ui.distance_culling)
		return (1);
	while (i < 3 + t1.isquad)
	{
		if (distance_culling(t2, t1.verts[i].pos, level->ui.render_distance))
			return (1);
		i++;
	}
	return (0);
}

void	reflection_culling(t_level *level, int i)
{
	t_vec3	avg;
	int		amount;
	int		k;

	level->all.tris[i].reflection_obj_all.tri_amount = 0;
	memset(&avg, 0, sizeof(t_vec3));
	k = -1;
	while (++k < 3 + level->all.tris[i].isquad)
		vec_add(&avg, avg, level->all.tris[i].verts[k].pos);
	vec_div(&avg, 3 + level->all.tris[i].isquad);
	amount = 0;
	k = -1;
	while (++k < level->all.tri_amount)
	{
		if ((level->all.tris[k].isgrid
				|| cull_behind(level->all.tris[i].normal,
					avg, level->all.tris[k]))
			&& face_distance_culling(level->all.tris[i],
				level->all.tris[k], level)
			&& reflection_backface(level->all.tris[k], level->all.tris[i]))
			level->all.tris[i].reflection_obj_all.tris[amount++]
				= level->all.tris[k];
	}
	level->all.tris[i].reflection_obj_all.tri_amount = amount;
}

void	opacity_culling(t_level *level, int i, t_obj *obj)
{
	int		amount;
	int		k;

	level->all.tris[i].opacity_obj_all.tri_amount = 0;
	amount = 0;
	k = 0;
	while (k < obj->tri_amount)
	{
		if ((obj->tris[k].isgrid
				|| cull_ahead(level->all.tris[i].normal,
					level->all.tris[i].verts[0].pos, obj->tris[k]))
			&& backface_culling(level->cam.pos, obj->tris[k]))
		{
			level->all.tris[i].opacity_obj_all.tris[amount]
				= obj->tris[k];
			amount++;
		}
		k++;
	}
	level->all.tris[i].opacity_obj_all.tri_amount = amount;
}

void	free_culling(t_level *level)
{
	int	i;

	i = 0;
	while (i < level->all.tri_amount)
	{
		free(level->all.tris[i].reflection_obj_all.tris);
		free(level->all.tris[i].reflection_obj_first_bounce.tris);
		free(level->all.tris[i].opacity_obj_all.tris);
		free(level->all.tris[i].shadow_faces.tris);
		i++;
	}
}

void	init_culling(t_level *level)
{
	int	i;

	i = -1;
	while (++i < level->all.tri_amount)
	{
		level->all.tris[i].texture = &level->texture;
		level->all.tris[i].reflection_obj_all.tris
			= (t_tri *)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].reflection_obj_first_bounce.tris
			= (t_tri *)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].opacity_obj_all.tris
			= (t_tri *)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].shadow_faces.tris
			= (t_tri *)malloc(sizeof(t_tri) * level->all.tri_amount);
		if (!level->all.tris[i].reflection_obj_first_bounce.tris
			|| !level->all.tris[i].opacity_obj_all.tris
			|| !level->all.tris[i].reflection_obj_all.tris
			|| !level->all.tris[i].shadow_faces.tris)
			ft_error("culling malloc fail");
		level->all.tris[i].dynamic = FALSE;
	}
	static_culling(level);
}

void	calculate_corner_vectors(t_vec3 corner[4], t_camera *cam)
{
	float	ym;
	float	xm;

	ym = -cam->fov_y / 2;
	xm = -cam->fov_x / 2;
	corner[0].x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	corner[0].y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	corner[0].z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
	ym = -cam->fov_y / 2;
	xm = cam->fov_x - cam->fov_x / 2;
	corner[1].x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	corner[1].y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	corner[1].z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
	ym = cam->fov_y - cam->fov_y / 2;
	xm = -cam->fov_x / 2;
	corner[2].x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	corner[2].y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	corner[2].z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
	ym = cam->fov_y - cam->fov_y / 2;
	xm = cam->fov_x - cam->fov_x / 2;
	corner[3].x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	corner[3].y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	corner[3].z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
}

void	skybox_culling(t_level *level, t_camera *cam,
										t_vec3 side_normals[4])
{
	int	visible_amount;
	int	i;

	visible_amount = 0;
	i = 0;
	while (i < level->sky.all.tri_amount)
	{
		if (cull_behind(cam->front, (t_vec3){0, 0, 0},
			level->sky.all.tris[i])
			&& frustrum_culling(side_normals,
				(t_vec3){0, 0, 0}, level->sky.all.tris[i]))
		{
			level->sky.visible.tris[visible_amount] = level->sky.all.tris[i];
			visible_amount++;
		}
		i++;
	}
	level->sky.visible.tri_amount = visible_amount;
}

void	static_culling(t_level *l)
{
	int	i;

	i = 0;
	while (i < l->all.tri_amount)
	{
		if (l->all.tris[i].reflectivity)
			reflection_culling(l, l->all.tris[i].index);
		shadow_face_culling(l, l->all.tris[i].index);
		if (l->all.tris[i].opacity
			&& l->all.tris[i].opacity_precise
			&& l->all.tris[i].refractivity < 0)
			opacity_culling(l, l->all.tris[i].index, &l->all);
		i++;
	}
}

void	cull_visible(t_level *l, t_vec3 side_normals[4])
{
	int	i;

	l->visible.tri_amount = 0;
	visible_request_merge(l, l->all.tri_amount);
	i = -1;
	while (++i < l->all.tri_amount)
	{
		if (l->all.tris[i].isgrid
			|| (cull_behind(l->cam.front, l->cam.pos, l->all.tris[i])
				&& frustrum_culling(side_normals, l->cam.pos, l->all.tris[i])
				&& (!l->ui.distance_culling
					|| distance_culling(l->all.tris[i], l->cam.pos,
						l->ui.render_distance))
				&& (!l->ui.backface_culling
					|| backface_culling(l->cam.pos, l->all.tris[i]))
				&& (!l->ui.occlusion_culling || !l->ui.backface_culling
					|| l->all.tris[i].opacity
					|| occlusion_culling(l->all.tris[i], l))))
			l->visible.tris[l->visible.tri_amount++] = l->all.tris[i];
	}
}

void	culling(t_level *l)
{
	t_vec3		corner[4];
	t_vec3		side_normals[4];
	int			i;

	calculate_corner_vectors(corner, &l->cam);
	calculate_side_normals(side_normals, corner);
	skybox_culling(l, &l->cam, side_normals);
	cull_visible(l, side_normals);
	i = -1;
	while (++i < l->visible.tri_amount)
	{
		if (l->visible.tris[i].reflectivity)
			reflection_culling_first_bounce(l, l->visible.tris[i].index);
		if (l->visible.tris[i].opacity_precise
			&& l->visible.tris[i].refractivity > 0)
			opacity_culling(l, l->visible.tris[i].index, &l->visible);
	}
}
