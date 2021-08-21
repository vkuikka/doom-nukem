/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wireframe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/05 16:44:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/21 22:22:23 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	pixel_put(int x, int y, unsigned int color, unsigned int *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return ;
	if (color == WF_SELECTED_COL
		|| (texture[x + (y * RES_X)] != WF_VERT_COL
			&& texture[x + (y * RES_X)] != WF_SELECTED_COL
			&& texture[x + (y * RES_X)] != WF_VISIBLE_COL
			&& texture[x + (y * RES_X)] != WF_VISIBLE_NORMAL_COL
			&& (texture[x + (y * RES_X)] != WF_NOT_QUAD_WARNING_COL
				|| color == WF_SELECTED_COL)))
		texture[x + (y * RES_X)] = color;
}

t_vec3	move2z(t_vec3 *p1, t_vec3 *p2)
{
	t_vec3	intersection;
	float	len;

	len = p1->z / (p2->z - p1->z);
	len *= 20;
	intersection.x = p1->x + len * (p2->x - p1->x);
	intersection.y = p1->y + len * (p2->y - p1->y);
	intersection.z = 0;
	return (intersection);
}

void	print_line(t_vec3 start, t_vec3 stop, unsigned int color, unsigned int *texture)
{
	t_vec3	step;
	t_vec3	pos;
	int		i;

	if (start.z < 0 && stop.z < 0)
		return ;
	else if (start.z < 0)
		start = move2z(&stop, &start);
	else if (stop.z < 0)
		stop = move2z(&start, &stop);
	i = 0;
	pos.x = start.x;
	pos.y = start.y;
	pos.z = 0;
	if (ft_abs(stop.x - start.x) > ft_abs(stop.y - start.y))
		step.z = ft_abs(stop.x - start.x);
	else
		step.z = ft_abs(stop.y - start.y);
	step.x = (stop.x - start.x) / (float)step.z;
	step.y = (stop.y - start.y) / (float)step.z;
	while (pos.z <= step.z && i < 10000)
	{
		pixel_put(pos.x, pos.y, color, texture);
		pos.x += step.x;
		pos.y += step.y;
		pos.z++;
		i++;
	}
}

void	rotate_vertex(float angle, t_vec3 *vertex, int axis)
{
	float	sin_angle;
	float	cos_angle;
	float	a;
	float	b;

	sin_angle = sin(angle);
	cos_angle = cos(angle);
	if (axis)
	{
		a = vertex->y;
		b = vertex->z;
		vertex->y = a * cos_angle - b * sin_angle;
		vertex->z = b * cos_angle + a * sin_angle;
	}
	else
	{
		a = vertex->x;
		b = vertex->z;
		vertex->x = a * cos_angle + b * sin_angle;
		vertex->z = b * cos_angle - a * sin_angle;
	}
}

void	pixel_put_force(int x, int y, int color, unsigned int *texture)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return ;
	if (texture[x + (y * RES_X)] != WF_SELECTED_COL)
		texture[x + (y * RES_X)] = color;
}

static void	put_vertex(t_vec3 vertex, int color, unsigned int *texture)
{
	int	a;
	int	b;

	if (vertex.z < 0)
		return ;
	a = -1;
	while (a < 2)
	{
		b = -1;
		while (b < 2)
		{
			pixel_put_force(vertex.x + a, vertex.y + b, color, texture);
			b++;
		}
		a++;
	}
}

void	camera_offset(t_vec3 *vertex, t_camera *cam)
{
	// move vertices to camera position
	vertex->x -= cam->pos.x;
	vertex->y -= cam->pos.y;
	vertex->z -= cam->pos.z;
	// rotate vertices around camera
	rotate_vertex(-cam->look_side, vertex, 0);
	rotate_vertex(-cam->look_up, vertex, 1);
	// add perspective
	vertex->x /= vertex->z / (RES_X / cam->fov_x);
	vertex->y /= vertex->z / (RES_Y / cam->fov_y);
	// move to center of screen
	vertex->x += RES_X / 2.0;
	vertex->y += RES_Y / 2.0;
}

void	put_normal(unsigned int *texture, t_level *level, t_tri tri, unsigned int color)
{
	t_vec3	normal;
	t_vec3	avg;
	int		amount;
	int		j;

	amount = 3 + tri.isquad;
	avg.x = 0;
	avg.y = 0;
	avg.z = 0;
	j = 0;
	while (j < amount)
	{
		avg.x += tri.verts[j].pos.x;
		avg.y += tri.verts[j].pos.y;
		avg.z += tri.verts[j].pos.z;
		j++;
	}
	avg.x /= amount;
	avg.y /= amount;
	avg.z /= amount;
	normal = tri.normal;
	normal.x = avg.x + normal.x * WF_NORMAL_LEN;
	normal.y = avg.y + normal.y * WF_NORMAL_LEN;
	normal.z = avg.z + normal.z * WF_NORMAL_LEN;
	camera_offset(&avg, &level->cam);
	camera_offset(&normal, &level->cam);
	put_vertex(avg, 0, texture);
	print_line(avg, normal, color, texture);
}

void	render_wireframe(unsigned int *texture, t_level *level, t_obj *obj,
														int is_visible)
{
	t_vec3	start;
	t_vec3	stop;
	int		amount;
	int		next;
	int		i;
	int		j;

	i = -1;
	while (++i < obj->tri_amount)
	{
		if (!level->ui.wireframe && (is_visible || !obj->tris[i].selected))
			continue ;
		amount = 3 + obj->tris[i].isquad;
		j = -1;
		while (++j < amount)
		{
			if (amount == 4)
				next = (int[4]){1, 3, 0, 2}[j];
			else
				next = (j + 1) % 3;
			start = obj->tris[i].verts[j].pos;
			stop = obj->tris[i].verts[next].pos;
			camera_offset(&start, &level->cam);
			camera_offset(&stop, &level->cam);
			if (obj->tris[i].selected)
				print_line(start, stop, WF_SELECTED_COL, texture);
			else if (is_visible)
				print_line(start, stop, WF_VISIBLE_COL, texture);
			else if (level->ui.show_quads && !obj->tris[i].isquad)
				print_line(start, stop, WF_NOT_QUAD_WARNING_COL, texture);
			else
				print_line(start, stop, WF_UNSELECTED_COL, texture);
			if (obj->tris[i].verts[j].selected)
				put_vertex(start, WF_SELECTED_COL, texture);
			else
				put_vertex(start, WF_VERT_COL, texture);
			if (!is_visible)
				find_closest_mouse(&start, &i, &j);
		}
		if (is_visible || !level->ui.wireframe_culling_visual)
			put_normal(texture, level, obj->tris[i], WF_VISIBLE_NORMAL_COL);
		else
			put_normal(texture, level, obj->tris[i], WF_NORMAL_COL);
	}
}

void	wireframe(unsigned int *texture, t_level *level)
{
	int	x;
	int	y;
	if (!level->ui.wireframe_on_top && level->ui.wireframe)
	{
		y = -1;
		while (++y < RES_Y)
		{
			x = -1;
			while (++x < RES_X)
				texture[x + (y * RES_X)] = WF_BACKGROUND_COL;
		}
	}
	if (level->ui.wireframe && level->ui.wireframe_culling_visual)
		render_wireframe(texture, level, &level->visible, TRUE);
	render_wireframe(texture, level, &level->all, FALSE);
}
