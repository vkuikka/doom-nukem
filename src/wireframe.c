/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/05 16:44:10 by rpehkone          #+#    #+#             */
/*   Updated: 2020/03/07 21:39:06 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	pixel_put(int x, int y, int color, t_window *window)
{
	if (x < 0 || y < 0 || x > RES_X || y > RES_Y)
		return;
	//window->depth_buffer[x + (y * (int)RES_X)] = dist;
	window->frame_buffer[x + (y * (int)RES_X)] = color;
}

void	print_line(t_vec3 start, t_vec3 stop, int color[2], t_window *window)
{
	t_vec3	step;
	t_vec3	pos;
	int		i;

	if (start.z < 0 || stop.z < 0)
		return ;
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
		pixel_put(pos.x, pos.y, crossfade(color[0], color[1], 0xff * (pos.z / step.z), 0), window);
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
	float fov = 500;

	//move vertices to camera position
	vertex->x -= level->pos[0];
	vertex->y -= level->pos[1];
	vertex->z -= level->pos[2];

	//rotate vertices around camera
	rotate_vertex(-1 * level->look_side, vertex, 0);
	rotate_vertex(-1 * level->look_up, vertex, 1);

	//add perspective
	vertex->x /= vertex->z / fov;
	vertex->y /= vertex->z / fov;

	//move to center of screen
	vertex->x += RES_X / 2;
	vertex->y += RES_Y / 2;
}

void	wireframe(t_window *window, t_level *level)
{
	t_vec3	start;
	t_vec3	stop;
	t_vec3	avg;

	ft_memset(window->frame_buffer, 0x99, RES_X * RES_Y * sizeof(int));
	ft_memset(window->depth_buffer, 0, RES_X * RES_Y * sizeof(int));

	for (int i = 0; i < level->obj[0].tri_amount; i++)
	{
		int amount = level->obj[0].tris[i].isquad ? 4 : 3;
		for (int j = 0; j < amount; j++)
		{
			start.x = level->obj[0].tris[i].verts[j].pos[0];
			start.y = level->obj[0].tris[i].verts[j].pos[1];
			start.z = level->obj[0].tris[i].verts[j].pos[2];
			stop.x = level->obj[0].tris[i].verts[(j + 1) % 3].pos[0];
			stop.y = level->obj[0].tris[i].verts[(j + 1) % 3].pos[1];
			stop.z = level->obj[0].tris[i].verts[(j + 1) % 3].pos[2];

			camera_offset(&start, level);
			camera_offset(&stop, level);

			int color[2] = {0x333333, 0x333333};
			print_line(start, stop, color, window);
			put_vertex(start, 0, window);
			put_vertex(stop, 0, window);
		}
		avg.x = 0;
		avg.y = 0;
		avg.z = 0;
		for (int j = 0; j < amount; j++)
		{
			avg.x += level->obj[0].tris[i].verts[j].pos[0];
			avg.y += level->obj[0].tris[i].verts[j].pos[1];
			avg.z += level->obj[0].tris[i].verts[j].pos[2];
		}
		avg.x /= amount;
		avg.y /= amount;
		avg.z /= amount;
		float normal_dir[3];
		vec_cross(normal_dir, level->obj[0].tris[i].v0v1, level->obj[0].tris[i].v0v2);
		vec_normalize(normal_dir);
		t_vec3 normal;
		normal.x = avg.x - normal_dir[0];
		normal.y = avg.y - normal_dir[1];
		normal.z = avg.z - normal_dir[2];
		camera_offset(&avg, level);
		camera_offset(&normal, level);
		int color[2] = {0x00ffff, 0x00ffff};
		print_line(avg, normal, color, window);
	}
}
