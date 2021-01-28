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

void	wireframe(t_window *window, t_level *level)
{
	t_vec3	start;
	t_vec3	stop;

	ft_memset(window->frame_buffer, 0, RES_X * RES_Y * sizeof(int));
	ft_memset(window->depth_buffer, 0, RES_X * RES_Y * sizeof(int));

	float fov = 500;
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

			//move vertices to camera position
			start.x -= level->pos[0];
			start.y -= level->pos[1];
			start.z -= level->pos[2];
			stop.x -= level->pos[0];
			stop.y -= level->pos[1];
			stop.z -= level->pos[2];

			//rotate vertices around camera
			rotate_vertex(-1 * level->look_side, &start, 0);
			rotate_vertex(-1 * level->look_up, &start, 1);
			rotate_vertex(-1 * level->look_side, &stop, 0);
			rotate_vertex(-1 * level->look_up, &stop, 1);

			//add perspective
			start.x /= start.z / fov;
			start.y /= start.z / fov;
			stop.x /= stop.z / fov;
			stop.y /= stop.z / fov;

			//move to center of screen
			start.x += RES_X / 2;
			start.y += RES_Y / 2;
			stop.x += RES_X / 2;
			stop.y += RES_Y / 2;

			int color[2] = {0xff0000, 0x0000ff};
			print_line(start, stop, color, window);
		}
	}
}
