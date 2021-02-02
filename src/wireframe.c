/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wireframe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/05 16:44:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/02/02 21:32:42 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	pixel_put(int x, int y, int color, t_window *window)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
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
	vertex->x -= level->pos.x;
	vertex->y -= level->pos.y;
	vertex->z -= level->pos.z;

	//rotate vertices around camera
	rotate_vertex(-level->look_side, vertex, 0);
	rotate_vertex(-level->look_up, vertex, 1);

	//add perspective
	vertex->x /= vertex->z / fov;
	vertex->y /= vertex->z / fov;

	//move to center of screen
	vertex->x += RES_X / 2;
	vertex->y += RES_Y / 2;
}

void	put_edit_mode(int mode, t_window *window)
{
	char mode_selector[3][14][14] =
	{{"..............",
	"..............",
	"..............",
	"..............",
	"..............",
	"......xxx.....",
	"......xxx.....",
	"......xxx.....",
	"..............",
	"..............",
	"..............",
	"..............",
	"..............",
	".............."},

	{"..............",
	"..............",
	"..............",
	"...x..........",
	"....x.........",
	".....x........",
	"......x.......",
	".......x......",
	"........x.....",
	".........x....",
	"..........x...",
	"...........x..",
	"..............",
	".............."},

	{"..............",
	"..............",
	"..xxxxxxxxxx..",
	"...xxxxxxxxx..",
	"....xxxxxxxx..",
	".....xxxxxxx..",
	"......xxxxxx..",
	".......xxxxx..",
	"........xxxx..",
	".........xxx..",
	"..........xx..",
	"...........x..",
	"..............",
	".............."}};

	for (int i = 0; i < 3; i++)
	{
		for (int x = 0; x < 14; x++)
		{
			for (int y = 0; y < 14; y++)
			{
				if (mode_selector[i][y][x] == '.')
				{
					if (mode == i)
						pixel_put(x + 2 + i + i * 14, y + 1, 0x6666ddff, window);
					else
						pixel_put(x + 2 + i + i * 14, y + 1, 0x777777ff, window);
				}
				else
					pixel_put(x + 2 + i + i * 14, y + 1, 0xccccccff, window);
			}
		}
	}
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 14 * 3 + 5; x++)
		{
			if (y == 0 || y == 15 || x == 0 || x == 1 || x == 16 || x == 31 || x == 46)
				pixel_put(x, y, 0x444444ff, window);
		}
	}
}

int		select_mode(int mode)
{
	const Uint8	*keys = SDL_GetKeyboardState(NULL);
	int			x;
	int			y;

	if (!SDL_GetRelativeMouseMode() && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (y <= 17 && (x >= 2 && x <= 17))
			return (0);
		else if (y <= 17 && (x >= 18 && x <= 32))
			return (1);
		else if (y <= 17 && (x >= 33 && x <= 47))
			return (2);
	}
	if (keys[SDL_SCANCODE_1])
		return (0);
	else if (keys[SDL_SCANCODE_2])
		return (1);
	else if (keys[SDL_SCANCODE_3])
		return (2);
	return (mode);
}

void	wireframe(t_window *window, t_level *level)
{
	static int		selected_face = 0;
	static t_vec3	selected_vert = {.x = 0, .y = 0, .z = 0};
	static int	selected = 0;
	static int	mode = 0;
	t_vec3		start;
	t_vec3		stop;
	t_vec3		avg;

	global_seginfo = "wireframe start\n";
	const Uint8	*keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_4])
		selected -= 1;
	if (keys[SDL_SCANCODE_5])
		selected += 1;
	selected = selected > level->obj[0].tri_amount ? 0 : selected;
	selected = selected < 0 ? level->obj[0].tri_amount : selected;

	ft_memset(window->frame_buffer, 0x99, RES_X * RES_Y * sizeof(int));
	ft_memset(window->depth_buffer, 0, RES_X * RES_Y * sizeof(int));

	global_seginfo = "wireframe loop\n";
	for (int i = 0; i < level->obj[0].tri_amount; i++)
	{
		int amount = level->obj[0].tris[i].isquad ? 4 : 3;
		for (int j = 0; j < amount; j++)
		{
			start.x = level->obj[0].tris[i].verts[j].pos.x;
			start.y = level->obj[0].tris[i].verts[j].pos.y;
			start.z = level->obj[0].tris[i].verts[j].pos.z;
			if (amount == 4)
			{
				stop.x = level->obj[0].tris[i].verts[(int[4]){1, 3, 0, 2}[j]].pos.x;
				stop.y = level->obj[0].tris[i].verts[(int[4]){1, 3, 0, 2}[j]].pos.y;
				stop.z = level->obj[0].tris[i].verts[(int[4]){1, 3, 0, 2}[j]].pos.z;
			}
			else
			{
				stop.x = level->obj[0].tris[i].verts[(j + 1) % 3].pos.x;
				stop.y = level->obj[0].tris[i].verts[(j + 1) % 3].pos.y;
				stop.z = level->obj[0].tris[i].verts[(j + 1) % 3].pos.z;
			}

			global_seginfo = "wireframe 1\n";
			camera_offset(&start, level);
			camera_offset(&stop, level);

			int color[2];
			if ((mode == 2 && i == selected) || (mode == 1 && i == selected && j == 1))
			{
				color[0] = 0xffaa00;
				color[1] = 0xffaa00;
			}
			else
			{
				color[0] = 0x333333;
				color[1] = 0x333333;
			}
			print_line(start, stop, color, window);
			if (mode == 0)
			{
				if (selected == i)
					put_vertex(start, 0xffaa00ff, window);
				else
					put_vertex(start, 0, window);
			}
			//put_vertex(stop, 0, window);
		}
		global_seginfo = "wireframe 2\n";
		avg.x = 0;
		avg.y = 0;
		avg.z = 0;
		for (int j = 0; j < amount; j++)
		{
			avg.x += level->obj[0].tris[i].verts[j].pos.x;
			avg.y += level->obj[0].tris[i].verts[j].pos.y;
			avg.z += level->obj[0].tris[i].verts[j].pos.z;
		}
		avg.x /= amount;
		avg.y /= amount;
		avg.z /= amount;
		t_vec3	normal_dir;
		vec_cross(&normal_dir, level->obj[0].tris[i].v0v1, level->obj[0].tris[i].v0v2);
		vec_normalize(&normal_dir);
		global_seginfo = "wireframe 3\n";
		t_vec3 normal;
		float normal_len = 0.3;
		normal.x = avg.x - normal_dir.x * normal_len;
		normal.y = avg.y - normal_dir.y * normal_len;
		normal.z = avg.z - normal_dir.z * normal_len;
		camera_offset(&avg, level);
		camera_offset(&normal, level);
		int color[2] = {0x00ffff, 0x00ffff};
		print_line(avg, normal, color, window);
		if (mode == 2)
		{
			if (selected == i)
				put_vertex(avg, 0xffaa00ff, window);
			else
				put_vertex(avg, 0, window);
		}
	}
	global_seginfo = "wireframe 4\n";
	mode = select_mode(mode);
	put_edit_mode(mode, window);
	global_seginfo = "wireframe end\n";
}
