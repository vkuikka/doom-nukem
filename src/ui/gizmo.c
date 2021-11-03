/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gizmo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/26 19:56:07 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/13 18:06:34 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	gizmo_pixel_put(int x, int y, int color, unsigned int *texture)
{
	if (x >= 0 && y >= 0 && x < RES_X && y < RES_Y)
		texture[x + (y * RES_X)] = color;
}

void	gizmo_print_line(t_vec3 start, t_vec3 stop, int color,
												unsigned int *pixels)
{
	t_vec3	step;
	t_vec3	pos;
	int		i;

	i = 0;
	if (start.z < 0 || stop.z < 0)
		return ;
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
		gizmo_pixel_put(pos.x, pos.y, color, pixels);
		pos.x += step.x;
		pos.y += step.y;
		pos.z++;
		i++;
	}
}

t_vec3	vec_sub_return(t_vec3 ve1, t_vec3 ve2)
{
	t_vec3	res;

	res.x = ve1.x - ve2.x;
	res.y = ve1.y - ve2.y;
	res.z = ve1.z - ve2.z;
	return (res);
}

static void	put_gizmo_box(t_vec3 vertex, int color, unsigned int *pixels)
{
	int	a;
	int	b;

	if (vertex.z < 0)
		return ;
	a = -6;
	while (a < 11)
	{
		b = -6;
		while (b < 11)
		{
			gizmo_pixel_put(vertex.x + a, vertex.y + b, color, pixels);
			b++;
		}
		a++;
	}
}

void	gizmo_render(t_level *level, unsigned int *pixels)
{
	t_vec3	avg;
	t_vec3	x;
	t_vec3	y;
	t_vec3	z;

	x = (t_vec3){-1.5, 0, 0};
	y = (t_vec3){0, -1.5, 0};
	z = (t_vec3){0, 0, -1.5};
	avg = level->ui.state.gizmo_pos;
	vec_add(&x, x, avg);
	vec_add(&y, y, avg);
	vec_add(&z, z, avg);
	camera_offset(&avg, &level->cam);
	camera_offset(&x, &level->cam);
	camera_offset(&y, &level->cam);
	camera_offset(&z, &level->cam);
	gizmo_print_line(avg, x, 0xff0000ff, pixels);
	gizmo_print_line(avg, y, 0x00ff00ff, pixels);
	gizmo_print_line(avg, z, 0x0000ffff, pixels);
	put_gizmo_box(x, 0xff0000ff, pixels);
	put_gizmo_box(y, 0x00ff00ff, pixels);
	put_gizmo_box(z, 0x0000ffff, pixels);
}

int	is_near(int a, int b, int range)
{
	float	diff;

	diff = a - b;
	return (-range <= diff && diff <= range);
}

static t_vec3	calc_move_screen_space(int index, int amount)
{
	int		get_dir[3];
	t_vec3	dir;

	get_dir[0] = 0;
	get_dir[1] = 0;
	get_dir[2] = 0;
	get_dir[index] = -1;
	dir.x = get_dir[0];
	dir.y = get_dir[1];
	dir.z = get_dir[2];
	vec_mult(&dir, amount / 100.);
	return (dir);
}

static void	gizmo_call_move(t_level *level, t_vec3 move_amount)
{
	if (level->ui.state.ui_location == UI_LOCATION_DOOR_ACTIVATION_BUTTON)
		door_activation_move(level, move_amount);
	else if (level->ui.state.ui_location == UI_LOCATION_LIGHT_EDITOR)
		move_light(level, move_amount);
	else if (level->ui.state.ui_location == UI_LOCATION_GAME_SETTINGS)
		game_logic_move_selected(level, move_amount);
	else
		obj_editor_input(level, move_amount);
}

static void	gizmo_set_mouse_location(t_level *level,
				int deltax, int deltay, int drag_direction)
{
	t_vec3	res;

	if (!level->ui.state.m1_drag
		&& level->ui.state.mouse_location >= MOUSE_LOCATION_GIZMO_X)
		level->ui.state.mouse_location = MOUSE_LOCATION_SELECTION;
	res = (t_vec3){0, 0, 0};
	if (deltax || deltay)
	{
		if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_Y)
			res = calc_move_screen_space(level->ui.state.mouse_location - 3,
					deltay * drag_direction);
		else if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_X
			|| level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_Z)
			res = calc_move_screen_space(level->ui.state.mouse_location - 3,
					deltax * drag_direction);
	}
	gizmo_call_move(level, res);
}

static void	gizmo_move_amount(t_level *level, t_ivec2 mouse, int drag_direction)
{
	static t_ivec2	prev_mouse;

	gizmo_set_mouse_location(level,
		prev_mouse.x - mouse.x, prev_mouse.y - mouse.y, drag_direction);
	prev_mouse = mouse;
}

void	gizmo_drag(t_level *level, t_vec3 x, t_vec3 y, t_vec3 z)
{
	static int	drag_direction = 1;
	t_ivec2		mouse;

	mouse = level->ui.state.mouse;
	if (!level->ui.state.m1_click)
		return (gizmo_move_amount(level, mouse, drag_direction));
	if (y.z > 0 && is_near(mouse.x, y.x, 9) && is_near(mouse.y, y.y, 9))
		level->ui.state.mouse_location = MOUSE_LOCATION_GIZMO_Y;
	if (y.z > 0 && is_near(mouse.x, y.x, 9) && is_near(mouse.y, y.y, 9))
		drag_direction = 1;
	else if (x.z > 0 && is_near(mouse.x, x.x, 9) && is_near(mouse.y, x.y, 9))
	{
		level->ui.state.mouse_location = MOUSE_LOCATION_GIZMO_X;
		drag_direction = 1;
		if (vec_dot((t_vec3){0, 0, -1}, level->cam.front) > 0)
			drag_direction = -1;
	}
	else if (z.z > 0 && is_near(mouse.x, z.x, 9) && is_near(mouse.y, z.y, 9))
	{
		level->ui.state.mouse_location = MOUSE_LOCATION_GIZMO_Z;
		drag_direction = -1;
		if (vec_dot((t_vec3){-1, 0, 0}, level->cam.front) > 0)
			drag_direction = 1;
	}
	gizmo_move_amount(level, mouse, drag_direction);
}

void	gizmo(t_level *level)
{
	t_vec3	x;
	t_vec3	y;
	t_vec3	z;
	t_vec3	avg;

	x = (t_vec3){-1.5, 0, 0};
	y = (t_vec3){0, -1.5, 0};
	z = (t_vec3){0, 0, -1.5};
	avg = level->ui.state.gizmo_pos;
	vec_add(&x, x, avg);
	vec_add(&y, y, avg);
	vec_add(&z, z, avg);
	camera_offset(&avg, &level->cam);
	camera_offset(&x, &level->cam);
	camera_offset(&y, &level->cam);
	camera_offset(&z, &level->cam);
	gizmo_drag(level, x, y, z);
}
