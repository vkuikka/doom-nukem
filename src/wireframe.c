/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wireframe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/05 16:44:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/03/31 20:25:43 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	pixel_put(int x, int y, int color, t_window *window)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return;
	if (color == WF_SELECTED_COL ||
		(window->frame_buffer[x + (y * RES_X)] != WF_SELECTED_COL &&
		window->frame_buffer[x + (y * RES_X)] != WF_VISIBLE_COL &&
		window->frame_buffer[x + (y * RES_X)] != WF_VISIBLE_NORMAL_COL &&
		(window->frame_buffer[x + (y * RES_X)] != WF_NOT_QUAD_WARNING_COL ||
		color == WF_SELECTED_COL)))
		window->frame_buffer[x + (y * RES_X)] = color;
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

void	print_line(t_vec3 start, t_vec3 stop, int color, t_window *window)
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
	step.z = ft_abs(stop.x - start.x) > ft_abs(stop.y - start.y) ?
			ft_abs(stop.x - start.x) : ft_abs(stop.y - start.y);
	step.x = (stop.x - start.x) / (float)step.z;
	step.y = (stop.y - start.y) / (float)step.z;
	while (pos.z <= step.z && i < 10000)
	{
		pixel_put(pos.x, pos.y, color, window);
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

void	put_vertex(t_vec3 vertex, int color, t_window *window)
{
	if (vertex.z < 0)
		return ;
	for (int a = -1; a < 2; a++)
	{
		for (int b = -1; b < 2; b++)
		{
			pixel_put(vertex.x + a, vertex.y + b, color, window);
		}
	}
}

void	camera_offset(t_vec3 *vertex, t_level *level)
{
	//move vertices to camera position
	vertex->x -= level->pos.x;
	vertex->y -= level->pos.y;
	vertex->z -= level->pos.z;

	//rotate vertices around camera
	rotate_vertex(-level->look_side, vertex, 0);
	rotate_vertex(-level->look_up, vertex, 1);

	//add perspective
	vertex->x /= vertex->z / RES_X;
	vertex->y /= vertex->z / RES_Y;

	//move to center of screen
	vertex->x += RES_X / 2.0;
	vertex->y += RES_Y / 2.0;
}


void	put_normal(t_window *window, t_level *level, t_tri tri, int color)
{
	t_vec3	avg;
	int		amount;

	amount = tri.isquad ? 4 : 3;
	avg.x = 0;
	avg.y = 0;
	avg.z = 0;
	for (int j = 0; j < amount; j++)
	{
		avg.x += tri.verts[j].pos.x;
		avg.y += tri.verts[j].pos.y;
		avg.z += tri.verts[j].pos.z;
	}
	avg.x /= amount;
	avg.y /= amount;
	avg.z /= amount;
	t_vec3	normal_dir;
	vec_cross(&normal_dir, tri.v0v1, tri.v0v2);
	vec_normalize(&normal_dir);
	t_vec3 normal;
	float normal_len = 0.3;
	normal.x = avg.x - normal_dir.x * normal_len;
	normal.y = avg.y - normal_dir.y * normal_len;
	normal.z = avg.z - normal_dir.z * normal_len;
	camera_offset(&avg, level);
	camera_offset(&normal, level);
	put_vertex(avg, 0, window);
	print_line(avg, normal, color, window);
}

void	render_wireframe(t_window *window, t_level *level, t_obj *obj, int is_visible)
{
	t_vec3		start;
	t_vec3		stop;

	for (int i = 0; i < obj->tri_amount; i++)
	{
		int amount = obj->tris[i].isquad ? 4 : 3;
		for (int j = 0; j < amount; j++)
		{
			int		next;
			if (amount == 4)
				next = (int[4]){1, 3, 0, 2}[j];
			else
				next = (j + 1) % 3;
			start = obj->tris[i].verts[j].pos;
			stop = obj->tris[i].verts[next].pos;
			global_seginfo = "wireframe 1\n";
			camera_offset(&start, level);
			camera_offset(&stop, level);
			if (obj->tris[i].verts[next].selected &&
				obj->tris[i].verts[j].selected)
				print_line(start, stop, WF_SELECTED_COL, window);
			else if (is_visible)
				print_line(start, stop, WF_VISIBLE_COL, window);
			else if (level->ui->show_quads && !obj->tris[i].isquad)
				print_line(start, stop, WF_NOT_QUAD_WARNING_COL, window);
			else
				print_line(start, stop, WF_UNSELECTED_COL, window);
			// if (mode == 0)
			{
				if (obj->tris[i].verts[j].selected)
					put_vertex(start, WF_SELECTED_COL, window);
				else
					put_vertex(start, 0, window);
			}
		}
		if (is_visible || !level->ui->wireframe_culling_visual)
			put_normal(window, level, obj->tris[i], WF_VISIBLE_NORMAL_COL);
		else
			put_normal(window, level, obj->tris[i], WF_NORMAL_COL);
	}
}

void	wireframe(t_window *window, t_level *level)
{
	global_seginfo = "wireframe start\n";
	if (!level->ui->wireframe_on_top)
		ft_memset(window->frame_buffer, WF_BACKGROUND_COL, RES_X * RES_Y * sizeof(int));
	if (level->ui->wireframe_culling_visual)
		render_wireframe(window, level, &level->visible, TRUE);
	render_wireframe(window, level, &level->all, FALSE);
	// for (int asd = 0; asd < level->all.tri_amount; asd++)
	// 	for (int qwe = 0; qwe < 3 + level->all.tris[asd].isquad; qwe++)
	// 		if (level->all.tris[asd].verts[qwe].selected)
	// 			printf("%d\n", asd);
	global_seginfo = "wireframe end\n";
}
