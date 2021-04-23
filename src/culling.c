/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   culling.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 17:50:56 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/23 14:44:07 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

int				cull_ahead(t_vec3 dir, t_vec3 pos, t_tri tri)
{
	t_vec3	vert;
	int		i;

	i = 0;
	while (i < 3 + tri.isquad)
	{
		vec_sub(&vert, tri.verts[i].pos, pos);
		if (vec_dot(dir, vert) <= 0)
			return (TRUE);
		i++;
	}
	return (FALSE);
}

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

void		free_reflection_culling(t_level *level)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		free(level->all.tris[i].reflection_obj_all->tris);
		free(level->all.tris[i].reflection_obj_all);
		free(level->all.tris[i].reflection_obj_first_bounce->tris);
		free(level->all.tris[i].reflection_obj_first_bounce);
	}
}

void		reflection_culling_first_bounce(t_level *level, int i)
{
	if (level->all.tris[i].reflectivity)
	{
		level->all.tris[i].reflection_obj_first_bounce->tri_amount = 0;

		t_vec3		avg_dir = {0, 0, 0};
		for (int o = 0; o < 3 + level->all.tris[i].isquad; o++)
			vec_add(&avg_dir, avg_dir, level->all.tris[i].verts[o].pos);
		vec_div(&avg_dir, 3 + level->all.tris[i].isquad);
		vec_sub(&avg_dir, avg_dir, level->cam.pos);

		t_vec3		pos;
		t_vec3		normal;
		normal = level->all.tris[i].normal;
		vec_mult(&normal, vec_dot(avg_dir, normal));

		pos = level->cam.pos;
		vec_add(&pos, pos, normal);
		vec_add(&pos, pos, normal);

		t_vec3		reflection;
		reflection = level->cam.front;
		normal = level->all.tris[i].normal;
		vec_mult(&normal, vec_dot(reflection, normal) * -2);
		vec_add(&reflection, reflection, normal);

		int amount = 0;
		for (int k = 0; k < level->all.tris[i].reflection_obj_all->tri_amount; k++)
		{
			if (level->all.tris[i].reflection_obj_all->tris[k].isgrid || cull_behind(reflection, pos, level->all.tris[i].reflection_obj_all->tris[k]))
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
			if ((level->all.tris[k].isgrid || cull_ahead(level->all.tris[i].normal, avg, level->all.tris[k])) && reflection_backface(level->all.tris[k], level->all.tris[i]))
			{
				level->all.tris[i].reflection_obj_all->tris[amount] = level->all.tris[k];
				amount++;
			}
		}
		level->all.tris[i].reflection_obj_all->tri_amount = amount;
	}
}

void		init_reflection_culling(t_level *level)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		level->all.tris[i].index = i;
		level->all.tris[i].reflection_obj_all = (t_obj*)malloc(sizeof(t_obj));
		level->all.tris[i].reflection_obj_all->tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].reflection_obj_all->tri_amount = level->all.tri_amount;
		level->all.tris[i].reflection_obj_first_bounce = (t_obj*)malloc(sizeof(t_obj));
		level->all.tris[i].reflection_obj_first_bounce->tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount);
		level->all.tris[i].reflection_obj_first_bounce->tri_amount = level->all.tri_amount;
	}
	for (int i = 0; i < level->all.tri_amount; i++)
		reflection_culling(level, i);
}

static void		calculate_side_normals(t_vec3 normal[4], t_vec3 corner[4])
{
	vec_cross(&normal[0], corner[2], corner[0]);	//left
	vec_cross(&normal[1], corner[1], corner[3]);	//right
	vec_cross(&normal[2], corner[0], corner[1]);	//top
	vec_cross(&normal[3], corner[3], corner[2]);	//bot
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
				level->visible.tris[visible_amount] = level->visible.tris[i];
				visible_amount++;
			}
		}
	}
	level->visible.tri_amount = visible_amount;
}
