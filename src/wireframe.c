/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wireframe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/05 16:44:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/03/25 23:08:32 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	pixel_put(int x, int y, int color, t_window *window)
{
	if (x < 0 || y < 0 || x >= RES_X || y >= RES_Y)
		return;
	if (window->frame_buffer[x + (y * RES_X)] != WF_SELECTED_COL &&
		(window->frame_buffer[x + (y * RES_X)] != WF_NOT_QUAD_WARNING ||
		color == WF_SELECTED_COL))
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
	vertex->x += RES_X / 2.0;
	vertex->y += RES_Y / 2.0;
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

int		find_select(t_ray vec, t_level *l)
{
	int		color;
	int		dist = -1;
	int		res;

	for (int j = 0; j < l->obj[0].tri_amount; j++)
	{
		float tmp;
		tmp = cast_face(l->obj[0].tris[j], vec, &color, NULL);
		if (tmp > 0 && (tmp < dist || dist == -1))
		{
			dist = tmp;
			res = j;
		}
	}
	for (int i = 0; i < 3 + l->obj[0].tris[res].isquad; i++)
	{
		if (l->obj[0].tris[res].verts[i].selected)
		{
			l->obj[0].tris[res].verts[i].selected = 0;
			l->obj[0].tris[res].isgrid = 0;
		}
		else
		{
			l->obj[0].tris[res].verts[i].selected = 1;
			l->obj[0].tris[res].isgrid = 1;
		}
	}
	return (res);
}

void	select_vert(t_level *l, int x, int y)
{
	t_ray	r;
	t_vec3	cam;
	t_vec3	up;
	t_vec3	side;

	float lon = -l->look_side + M_PI/2;
	float lat = -l->look_up;

	rot_cam(&cam, lon, lat);
	rot_cam(&up, lon, lat + (M_PI / 2));
	vec_cross(&side, up, cam);

	float ym = (1.0 / RES_Y * y - 0.5);
	float xm = (1.0 / RES_X * x - 0.5);

	r.pos.x = l->pos.x;
	r.pos.y = l->pos.y;
	r.pos.z = l->pos.z;
	r.dir.x = cam.x + up.x * ym + side.x * xm;
	r.dir.y = cam.y + up.y * ym + side.y * xm;
	r.dir.z = cam.z + up.z * ym + side.z * xm;

	find_select(r, l);
}

int		select_mode(int mode, t_level *l)
{
	static int	pressed = 0;
	const Uint8	*keys = SDL_GetKeyboardState(NULL);
	int			x;
	int			y;

	if (!pressed && !SDL_GetRelativeMouseMode() && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (y <= 17 && (x >= 2 && x <= 17))
			return (0);
		else if (y <= 17 && (x >= 18 && x <= 32))
			return (1);
		else if (y <= 17 && (x >= 33 && x <= 47))
			return (2);
		else
			select_vert(l, x, y);
		pressed = TRUE;
	}
	if (!(SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		pressed = 0;
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
	static int	selected = 0;
	static int	mode = 0;
	t_vec3		start;
	t_vec3		stop;
	t_vec3		avg;

	global_seginfo = "wireframe start\n";
	const Uint8	*keys = SDL_GetKeyboardState(NULL);

	ft_memset(window->frame_buffer, WF_BACKGROUND_COL, RES_X * RES_Y * sizeof(int));
	ft_memset(window->depth_buffer, 0, RES_X * RES_Y * sizeof(int));

	global_seginfo = "wireframe loop\n";
	for (int i = 0; i < level->obj[0].tri_amount; i++)
	{
		int amount = level->obj[0].tris[i].isquad ? 4 : 3;
		for (int j = 0; j < amount; j++)
		{
			int		next;
			if (amount == 4)
				next = (int[4]){1, 3, 0, 2}[j];
			else
				next = (j + 1) % 3;

			start.x = level->obj[0].tris[i].verts[j].pos.x;
			start.y = level->obj[0].tris[i].verts[j].pos.y;
			start.z = level->obj[0].tris[i].verts[j].pos.z;
			stop.x = level->obj[0].tris[i].verts[next].pos.x;
			stop.y = level->obj[0].tris[i].verts[next].pos.y;
			stop.z = level->obj[0].tris[i].verts[next].pos.z;

			global_seginfo = "wireframe 1\n";
			camera_offset(&start, level);
			camera_offset(&stop, level);

			if (level->obj[0].tris[i].verts[next].selected &&
				level->obj[0].tris[i].verts[j].selected)
				print_line(start, stop, WF_SELECTED_COL, window);
			else if (level->ui->show_quads && !level->obj[0].tris[i].isquad)
				print_line(start, stop, WF_NOT_QUAD_WARNING, window);
			else
				print_line(start, stop, WF_UNSELECTED_COL, window);
			if (mode == 0)
			{
				if (level->obj[0].tris[i].verts[j].selected)
					put_vertex(start, WF_SELECTED_COL, window);
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
		print_line(avg, normal, WF_NORMAL_COL, window);
		if (mode == 2)
		{
			if (selected == i)
				put_vertex(avg, WF_SELECTED_COL, window);
			else
				put_vertex(avg, 0, window);
		}
	}
	global_seginfo = "wireframe 4\n";
	mode = select_mode(mode, level);
	// for (int asd = 0; asd < level->obj[0].tri_amount; asd++)
	// 	for (int qwe = 0; qwe < 3 + level->obj[0].tris[asd].isquad; qwe++)
	// 		if (level->obj[0].tris[asd].verts[qwe].selected)
	// 			printf("%d\n", asd);
	put_edit_mode(mode, window);
	global_seginfo = "wireframe end\n";
}