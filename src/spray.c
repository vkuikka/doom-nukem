/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spray.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/01 18:48:35 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/07 15:58:33 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	draw_line(t_level *l, t_vec2 line[2], t_tri *tri, float y_percent)
{
	t_vec2	increment;
	t_vec2	texture;
	int		spray_coord;
	int		texture_coord;
	int		steps;
	int		i;
	t_vec2	point;

	i = 0;
	texture.x = line[0].x;
	texture.y = line[0].y;
	steps = fmax(ft_abs(line[1].x - line[0].x), ft_abs(line[1].y - line[0].y))
		* SPRAY_LINE_PRECISION;
	if (!steps)
		return ;
	increment.x = (line[1].x - line[0].x) / (float)steps;
	increment.y = (line[1].y - line[0].y) / (float)steps;
	while (i <= steps && i < 99999)
	{
		if (texture.x < l->texture.width && texture.x > 0
			&& texture.y < l->texture.height && texture.y > 0)
		{
			texture_coord = (int)texture.x + (int)texture.y * l->texture.width;
			if (texture_coord < l->texture.width * l->texture.height
				&& texture_coord >= 0)
			{
				spray_coord = (int)(l->spray.width * (float)i / steps)
					+ (int)(l->spray.height * y_percent) * l->spray.width;
				if (spray_coord < l->spray.width * l->spray.height
					&& spray_coord >= 0 && l->spray.image[spray_coord] << 8 * 3 != 0
					&& l->texture.image[texture_coord] << 8 * 3 != 0)
				{
					point.x = texture.x / l->texture.width;
					point.y = 1 - texture.y / l->texture.height;
					l->spray.image[spray_coord]
						= (l->spray.image[spray_coord] >> 8 << 8) + 0xff;
					if (point_in_tri(point, tri->verts[0].txtr,
							tri->verts[1].txtr, tri->verts[2].txtr)
						|| point_in_tri(point, tri->verts[3].txtr,
							tri->verts[1].txtr, tri->verts[2].txtr))
						l->spray_overlay[texture_coord]
							= l->spray.image[spray_coord];
				}
			}
		}
		texture.x += increment.x;
		texture.y += increment.y;
		i++;
	}
}

static void	square_step(t_vec2 square[4], t_level *l, t_tri *tri, float step)
{
	t_vec2	line[2];

	line[0] = square[1];
	line[1] = square[3];
	vec2_mult(&line[0], -step);
	vec2_mult(&line[1], -step);
	vec2_add(&line[0], square[0], line[0]);
	vec2_add(&line[1], square[2], line[1]);
	draw_line(l, line, tri, step);
}

static void	draw_square(t_level *l, t_vec2 square[4], t_tri *tri)
{
	int		left_steps;
	int		right_steps;
	int		steps;
	int		i;

	if (ft_abs(square[1].x - square[0].x) > ft_abs(square[1].y - square[0].y))
		left_steps = ft_abs(square[1].x - square[0].x);
	else
		left_steps = ft_abs(square[1].y - square[0].y);
	if (ft_abs(square[3].x - square[2].x) > ft_abs(square[3].y - square[2].y))
		right_steps = ft_abs(square[3].x - square[2].x);
	else
		right_steps = ft_abs(square[3].y - square[2].y);
	if (!left_steps || !right_steps)
		return ;
	if (left_steps > right_steps)
		steps = left_steps;
	else
		steps = right_steps;
	steps *= SPRAY_LINE_PRECISION;
	i = -1;
	while (++i <= steps && i < 99999)
		square_step(square, l, tri, (float)i / steps);
}

t_vec2	uv_to_2d(t_tri tri, t_vec2 uv, int isquad)
{
	t_vec2	av0;
	t_vec2	av1;
	t_vec2	av2;
	t_vec2	res;

	if (isquad)
		av0 = tri.verts[3].txtr;
	else
		av0 = tri.verts[0].txtr;
	av1 = tri.verts[1].txtr;
	av2 = tri.verts[2].txtr;
	vec2_mult(&av0, 1 - uv.x - uv.y);
	vec2_mult(&av1, uv.y);
	vec2_mult(&av2, uv.x);
	res = av0;
	vec2_add(&res, res, av1);
	vec2_add(&res, res, av2);
	return (res);
}

static int	cast_uv(t_tri t, t_ray ray, t_vec2 *uv)
{
	t_vec3	vec;
	t_vec3	tvec;
	float	u;
	float	v;
	float	tmp;

	vec_cross(&vec, ray.dir, t.v0v2);
	tmp = 1 / vec_dot(vec, t.v0v1);
	vec_sub(&tvec, ray.pos, t.verts[0].pos);
	u = vec_dot(tvec, vec) * tmp;
	vec_cross(&vec, tvec, t.v0v1);
	v = vec_dot(ray.dir, vec) * tmp;
	tmp = vec_dot(vec, t.v0v2) * tmp;
	if (tmp > SPRAY_MAX_DIST || tmp < 0)
		return (0);
	uv->x = u;
	uv->y = v;
	return (1);
}

static int	raycast_face_pos(t_ray *r, t_obj *object)
{
	float	dist;
	float	tmp;
	int		i;
	int		new_hit;

	i = 0;
	dist = FLT_MAX;
	new_hit = -1;
	while (i < object->tri_amount)
	{
		tmp = cast_face(object->tris[i], *r, NULL);
		if (tmp > 0 && tmp < dist)
		{
			dist = tmp;
			new_hit = object->tris[i].index;
		}
		i++;
	}
	if (new_hit == -1)
		return (-1);
	vec_mult(&r->dir, dist);
	return (new_hit);
}

t_vec2	corner_cam_diff(int corner, t_camera *cam)
{
	int		diff;
	t_vec2	cam_diff;

	diff = fmin(RES_Y, RES_X) * (SPRAY_FROM_VIEW_SIZE / 2);
	if (corner == 0)
	{
		cam_diff.y = cam->fov_y / RES_Y * (RES_Y / 2 - diff) - cam->fov_y / 2;
		cam_diff.x = cam->fov_x / RES_X * (RES_X / 2 - diff) - cam->fov_x / 2;
	}
	else if (corner == 1)
	{
		cam_diff.y = cam->fov_y / RES_Y * (RES_Y / 2 + diff) - cam->fov_y / 2;
		cam_diff.x = cam->fov_x / RES_X * (RES_X / 2 - diff) - cam->fov_x / 2;
	}
	else if (corner == 2)
	{
		cam_diff.y = cam->fov_y / RES_Y * (RES_Y / 2 - diff) - cam->fov_y / 2;
		cam_diff.x = cam->fov_x / RES_X * (RES_X / 2 + diff) - cam->fov_x / 2;
	}
	else
	{
		cam_diff.y = cam->fov_y / RES_Y * (RES_Y / 2 + diff) - cam->fov_y / 2;
		cam_diff.x = cam->fov_x / RES_X * (RES_X / 2 + diff) - cam->fov_x / 2;
	}
	return (cam_diff);
}

void	move_cam(t_level *l, t_camera *cam, int hit, t_ray r)
{
	t_vec3	tmp;

	vec_add(&cam->pos, r.pos, r.dir);
	tmp = l->all.tris[hit].normal;
	vec_mult(&tmp, l->ui.spray_size);
	vec_add(&cam->pos, cam->pos, tmp);
	cam->front = l->all.tris[hit].normal;
	vec_mult(&cam->front, -1);
	tmp.x = 0;
	tmp.y = -1;
	tmp.z = 0;
	vec_cross(&cam->side, cam->front, tmp);
	vec_normalize(&cam->side);
	vec_cross(&cam->up, cam->front, cam->side);
	vec_normalize(&cam->up);
}

void	rot_to_corner(t_ray *r, t_camera cam, t_vec2 cam_diff)
{
	r->dir = cam.front;
	r->pos = cam.pos;
	r->dir.x += cam.up.x * cam_diff.y + cam.side.x * cam_diff.x;
	r->dir.y += cam.up.y * cam_diff.y + cam.side.y * cam_diff.x;
	r->dir.z += cam.up.z * cam_diff.y + cam.side.z * cam_diff.x;
}

static int	face_in_front(t_camera *cam, t_level *level)
{
	t_vec2	cam_diff;
	t_ray	r;
	int		hit;

	if (!level->ui.spray_from_view)
	{
		cam->fov_y = M_PI / 2;
		cam->fov_x = M_PI / 2 * ((float)RES_X / RES_Y);
	}
	cam_diff.y = cam->fov_y / RES_Y * (RES_Y / 2) - cam->fov_y / 2;
	cam_diff.x = cam->fov_x / RES_X * (RES_X / 2) - cam->fov_x / 2;
	rot_to_corner(&r, *cam, cam_diff);
	hit = raycast_face_pos(&r, &level->all);
	if (hit != -1 && !level->ui.spray_from_view)
		move_cam(level, cam, hit, r);
	return (hit);
}

void	spray(t_camera cam, t_level *level)
{
	t_vec2	uv;
	t_ray	r;
	t_vec2	corner[4];
	int		hit;
	int		i;

	hit = face_in_front(&cam, level);
	if (hit == -1)
		return ;
	i = -1;
	while (++i < 4)
	{
		rot_to_corner(&r, cam, corner_cam_diff(i, &cam));
		if (!cast_uv(level->all.tris[hit], r, &uv))
			return ;
		uv = uv_to_2d(level->all.tris[hit], uv, 0);
		corner[i].x = uv.x;
		corner[i].y = 1.0 - uv.y;
		corner[i].x *= level->texture.width;
		corner[i].y *= level->texture.height;
	}
	vec2_sub(&corner[1], corner[0], corner[1]);
	vec2_sub(&corner[3], corner[2], corner[3]);
	draw_square(level, corner, &level->all.tris[hit]);
}
