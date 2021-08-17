/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 17:50:56 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/28 20:59:58 vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

/*
**	Returns false if whole face is ahead
*/
int				cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri)
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

/*
**	Returns false if whole face is behind
*/
int				cull_behind(t_vec3 dir, t_vec3 pos, t_tri tri)
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

static int		fov_culling(t_vec3 side_normal[4], t_vec3 pos, t_tri tri)
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
	i = 0;
	while (i < 4)
	{
		if (out[0][i] && out[1][i] && out[2][i] && out[3][i])
			return (0);
		i++;
	}
	return (1);
}

static int		distance_culling(t_tri tri, t_vec3 player, float render_distance)
{
	t_vec3	v;
	float	min = FLT_MAX;
	float	max = 0;
	float	len;

	for (int i = 0; i < 3 + tri.isquad; i++)
	{
		vec_sub(&v, tri.verts[i].pos, player);
		len = vec_length(v);
		if (len < min)
			min = len;
		if (len > max)
			max = len;
	}
	if (min < render_distance)
		return (1);
	t_vec3	v1v2;
	vec_sub(&v1v2, tri.verts[2].pos, tri.verts[1].pos);
	min = vec_length(v1v2);
	len = vec_length(tri.v0v1);
	if (len < min)
		len = min;
	min = vec_length(tri.v0v2);
	if (len < min)
		len = min;
	if (tri.isquad)
	{
		t_vec3	v0v3;
		t_vec3	v1v3;
		t_vec3	v2v3;
		vec_sub(&v0v3, tri.verts[3].pos, tri.verts[0].pos);
		vec_sub(&v1v3, tri.verts[3].pos, tri.verts[1].pos);
		vec_sub(&v2v3, tri.verts[3].pos, tri.verts[2].pos);
		min = vec_length(v0v3);
		if (len < min)
			len = min;
		min = vec_length(v1v2);
		if (len < min)
			len = min;
		min = vec_length(v1v3);
		if (len < min)
			len = min;
	}
	return (max < len);
}

static int		backface_culling(t_vec3 pos, t_tri tri)
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

static int	reflection_backface(t_tri t1, t_tri t2)
{
	int i;

	i = 0;
	while (i < 3 + t2.isquad)
	{
		if (backface_culling(t2.verts[i].pos, t1))
			return (1);
		i++;
	}
	return (0);
}

static void		calculate_side_normals(t_vec3 normal[4], t_vec3 corner[4])
{
	vec_cross(&normal[0], corner[2], corner[0]);	//left
	vec_cross(&normal[1], corner[1], corner[3]);	//right
	vec_cross(&normal[2], corner[0], corner[1]);	//top
	vec_cross(&normal[3], corner[3], corner[2]);	//bot
}

void			shadow_face_culling(t_level *level, int i)
{
	t_tri		target;
	t_vec3		avg = {0, 0, 0};
	t_vec3		tmp[2];
	t_vec3		v[4];

	target = level->all.tris[i];
	for (int o = 0; o < 3 + target.isquad; o++)
		vec_add(&avg, avg, target.verts[o].pos);
	vec_div(&avg, 3 + target.isquad);

	tmp[0] = level->ui.sun_dir;
	vec_sub(&tmp[1], target.verts[0].pos, target.verts[1].pos);
	vec_cross(&v[0], tmp[1], tmp[0]);
	if (target.isquad)
	{
		vec_sub(&tmp[1], target.verts[1].pos, target.verts[3].pos);
		vec_cross(&v[1], tmp[1], tmp[0]);

		vec_sub(&tmp[1], target.verts[3].pos, target.verts[2].pos);
		vec_cross(&v[3], tmp[1], tmp[0]);

		vec_sub(&tmp[1], target.verts[2].pos, target.verts[0].pos);
		vec_cross(&v[2], tmp[1], tmp[0]);
	}
	else
	{
		vec_sub(&tmp[1], target.verts[1].pos, target.verts[2].pos);
		vec_cross(&v[1], tmp[1], tmp[0]);
		vec_sub(&tmp[1], target.verts[2].pos, target.verts[0].pos);
		vec_cross(&v[2], tmp[1], tmp[0]);
		v[3] = v[0];
		target.verts[3].pos = target.verts[0].pos;
	}
	int amount = 0;
	for (int k = 0; k < level->all.tri_amount; k++)
	{
		if (k != i && cull_behind(level->all.tris[i].normal, avg, level->all.tris[k]))
		{
			if (level->all.tris[k].isgrid || target.isgrid ||
				(cull_behind(v[0], target.verts[0].pos, level->all.tris[k]) &&
				cull_behind(v[1], target.verts[1].pos, level->all.tris[k]) &&
				cull_behind(v[2], target.verts[2].pos, level->all.tris[k]) &&
				cull_behind(v[3], target.verts[3].pos, level->all.tris[k])))
			{
				level->all.tris[i].shadow_faces->tris[amount] = level->all.tris[k];
				amount++;
			}
		}
	}
	level->all.tris[i].shadow_faces->tri_amount = amount;
}

void			reflection_culling_first_bounce(t_level *level, int i)
{
	t_vec3		avg_dir = {0, 0, 0};
	t_vec3		pos;
	t_vec3		normal;
	t_vec3		corner[4];
	t_vec3		side_normals[4];

	if (level->all.tris[i].reflectivity)
	{
		level->all.tris[i].reflection_obj_first_bounce->tri_amount = 0;

		for (int o = 0; o < 3 + level->all.tris[i].isquad; o++)
			vec_add(&avg_dir, avg_dir, level->all.tris[i].verts[o].pos);
		vec_div(&avg_dir, 3 + level->all.tris[i].isquad);
		vec_sub(&avg_dir, avg_dir, level->cam.pos);

		normal = level->all.tris[i].normal;
		vec_mult(&normal, vec_dot(avg_dir, normal));
		pos = level->cam.pos;
		vec_add(&pos, pos, normal);
		vec_add(&pos, pos, normal);
		avg_dir = level->cam.front;
		normal = level->all.tris[i].normal;
		vec_mult(&normal, vec_dot(avg_dir, normal) * -2);
		vec_add(&avg_dir, avg_dir, normal);

		corner[0] = level->all.tris[i].verts[0].pos;
		corner[1] = level->all.tris[i].verts[1].pos;
		corner[2] = level->all.tris[i].verts[2].pos;
		corner[3] = level->all.tris[i].verts[3].pos;
		vec_sub(&corner[0], pos, corner[0]);
		vec_sub(&corner[1], pos, corner[1]);
		vec_sub(&corner[2], pos, corner[2]);
		vec_sub(&corner[3], pos, corner[3]);
		vec_normalize(&corner[0]);
		vec_normalize(&corner[1]);
		vec_normalize(&corner[2]);
		vec_normalize(&corner[3]);
		calculate_side_normals(side_normals, corner);
		if (!level->all.tris[i].isquad)
			side_normals[3] = side_normals[0];

		int amount = 0;
		for (int k = 0; k < level->all.tris[i].reflection_obj_all->tri_amount; k++)
		{
			if ((level->all.tris[i].reflection_obj_all->tris[k].isgrid || cull_behind(avg_dir, pos, level->all.tris[i].reflection_obj_all->tris[k])) &&
				fov_culling(side_normals, pos, level->all.tris[i].reflection_obj_all->tris[k]))
			{
				level->all.tris[i].reflection_obj_first_bounce->tris[amount] = level->all.tris[i].reflection_obj_all->tris[k];
				amount++;
			}
		}
		level->all.tris[i].reflection_obj_first_bounce->tri_amount = amount;
	}
}

void		reflection_culling(t_level *level, int i)
{
	if (level->all.tris[i].reflectivity)
	{
		level->all.tris[i].reflection_obj_all->tri_amount = 0;
		t_vec3 avg = {0, 0, 0};
		for (int o = 0; o < 3 + level->all.tris[i].isquad; o++)
			vec_add(&avg, avg, level->all.tris[i].verts[o].pos);
		vec_div(&avg, 3 + level->all.tris[i].isquad);
		int amount = 0;
		for (int k = 0; k < level->all.tri_amount; k++)
		{
			if ((level->all.tris[k].isenemy || level->all.tris[k].isgrid ||
					cull_behind(level->all.tris[i].normal, avg, level->all.tris[k])) &&
					reflection_backface(level->all.tris[k], level->all.tris[i]))
			{
				level->all.tris[i].reflection_obj_all->tris[amount] = level->all.tris[k];
				amount++;
			}
		}
		level->all.tris[i].reflection_obj_all->tri_amount = amount;
	}
}

void		opacity_culling(t_level *level, int i)
{
	level->all.tris[i].opacity_obj_all->tri_amount = 0;
	t_vec3 avg = {0, 0, 0};
	for (int o = 0; o < 3 + level->all.tris[i].isquad; o++)
		vec_add(&avg, avg, level->all.tris[i].verts[o].pos);
	vec_div(&avg, 3 + level->all.tris[i].isquad);
	int amount = 0;
	for (int k = 0; k < level->all.tri_amount; k++)
	{
		if ((level->all.tris[k].isenemy || level->all.tris[k].isgrid ||
				cull_ahead(level->all.tris[i].normal, avg, level->all.tris[k])) &&
				backface_culling(level->cam.pos, level->all.tris[k]))
		{
			level->all.tris[i].opacity_obj_all->tris[amount] = level->all.tris[k];
			amount++;
		}
	}
	level->all.tris[i].opacity_obj_all->tri_amount = amount;
}

void		free_culling(t_level *level)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		free(level->all.tris[i].reflection_obj_all->tris);
		free(level->all.tris[i].reflection_obj_all);
		free(level->all.tris[i].reflection_obj_first_bounce->tris);
		free(level->all.tris[i].reflection_obj_first_bounce);
		free(level->all.tris[i].opacity_obj_all->tris);
		free(level->all.tris[i].opacity_obj_all);
		free(level->all.tris[i].shadow_faces->tris);
		free(level->all.tris[i].shadow_faces);
	}
}

void		init_culling(t_level *level)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		level->all.tris[i].index = i;
		/*
		level->all.tris[i].reflection_obj_all = (t_obj*)malloc(sizeof(t_obj));
		level->all.tris[i].reflection_obj_all->tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].reflection_obj_all->tri_amount = 0;
		level->all.tris[i].reflection_obj_first_bounce = (t_obj*)malloc(sizeof(t_obj));
		level->all.tris[i].reflection_obj_first_bounce->tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].reflection_obj_first_bounce->tri_amount = 0;
		level->all.tris[i].opacity_obj_all = (t_obj*)malloc(sizeof(t_obj));
		level->all.tris[i].opacity_obj_all->tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].opacity_obj_all->tri_amount = 0;
		level->all.tris[i].shadow_faces = (t_obj*)malloc(sizeof(t_obj));
		level->all.tris[i].shadow_faces->tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].shadow_faces->tri_amount = 0;
		*/
	}
	// for (int i = 0; i < level->all.tri_amount; i++)
	// {
	// 	reflection_culling(level, i);
	// 	opacity_culling(level, i);
	// }
}

static void		calculate_corner_vectors(t_vec3 corner[4], t_camera *cam)
{
	float ym;
	float xm;

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

static void		skybox_culling(t_level *level, t_camera *cam, t_vec3 side_normals[4])
{
	int visible_amount;

	visible_amount = 0;
	for (int i = 0; i < level->sky.all.tri_amount; i++)
	{
		if (cull_behind(cam->front, (t_vec3){0, 0, 0}, level->sky.all.tris[i]) && fov_culling(side_normals, (t_vec3){0, 0, 0}, level->sky.all.tris[i]))
		{
			level->sky.visible.tris[visible_amount] = level->sky.all.tris[i];
			visible_amount++;
		}
	}
	level->sky.visible.tri_amount = visible_amount;
}

void			culling(t_level *level)
{
	t_vec3		corner[4];
	t_vec3		side_normals[4];
	t_camera	*cam;

	cam = &level->cam;
	calculate_corner_vectors(corner, cam);
	calculate_side_normals(side_normals, corner);
	skybox_culling(level, cam, side_normals);
	int visible_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].isgrid ||
			(cull_behind(cam->front, cam->pos, level->all.tris[i]) && fov_culling(side_normals, cam->pos, level->all.tris[i]) &&
			(!level->ui.distance_culling || level->all.tris[i].disable_distance_culling || distance_culling(level->all.tris[i], cam->pos, level->ui.render_distance)) &&
			(!level->ui.backface_culling || level->all.tris[i].disable_backface_culling || backface_culling(cam->pos, level->all.tris[i]))))
		{
			level->visible.tris[visible_amount] = level->all.tris[i];
			visible_amount++;
		}
	}
	level->visible.tri_amount = visible_amount;
	if (level->ui.backface_culling)
	{
		visible_amount = 0;
		for (int i = 0; i < level->visible.tri_amount; i++)
		{
			if (level->visible.tris[i].isgrid || occlusion_culling(level->visible.tris[i], level))
			{
				reflection_culling_first_bounce(level, level->visible.tris[i].index);
				shadow_face_culling(level, level->visible.tris[i].index);
				opacity_culling(level, level->visible.tris[i].index);
				level->visible.tris[visible_amount] = level->visible.tris[i];
				visible_amount++;
			}
		}
	}
	level->visible.tri_amount = visible_amount;
}
