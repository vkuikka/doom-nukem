/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_path.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/20 17:41:32 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/20 17:41:32 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	draw_camera_path(char *str, t_camera_path *path,
					unsigned int *texture, t_level *level)
{
	t_vec3	a;
	t_vec3	b;
	int		i;
	char	buf[100];

	if (!path->amount)
		return ;
	i = 0;
	while (i < path->amount)
	{
		sprintf(buf, "%s %d", str, i + 1);
		render_text_3d(buf, path->pos[i].pos,
			UI_LEVEL_SETTINGS_TEXT_COLOR, level);
		b = path->pos[i].pos;
		camera_offset(&b, &level->cam);
		if (i)
			print_line(a, b, UI_LEVEL_SETTINGS_TEXT_COLOR, texture);
		a = b;
		i++;
	}
	if (!path->loop)
		return ;
	b = path->pos[0].pos;
	camera_offset(&b, &level->cam);
	print_line(a, b, UI_LEVEL_SETTINGS_TEXT_COLOR, texture);
}

void	cam_lerp(t_camera *cam, t_player_pos a, t_player_pos b, float f)
{
	cam->pos = vec_interpolate(a.pos, b.pos, f);
	cam->look_side = lerp(a.look_side, b.look_side, f);
	cam->look_up = lerp(a.look_up, b.look_up, f);
}

t_vec3	vec_bezier_middle(t_vec3 a, t_vec3 b, t_vec3 c, float f)
{
	a = vec_interpolate(a, b, 0.5);
	c = vec_interpolate(c, b, 0.5);
	a = vec_interpolate(a, b, f);
	c = vec_interpolate(b, c, f);
	return (vec_interpolate(a, c, f));
}

float	bezier_float_middle(float a, float b, float c, float f)
{
	a = lerp(a, b, 0.5);
	c = lerp(c, b, 0.5);
	a = lerp(a, b, f);
	c = lerp(b, c, f);
	return (lerp(a, c, f));
}

void	camera_path_get_pos_bezier(t_camera_path *path, t_camera *cam, float time)
{
	float	elem_size;
	float	node_time;
	int		node_amount;
	t_ivec3	id;

	node_amount = path->amount;
	if (!path->loop)
		node_amount -= 2;
	id.x = ((int)(node_amount * time) + 0) % path->amount;
	id.y = ((int)(node_amount * time) + 1) % path->amount;
	id.z = ((int)(node_amount * time) + 2) % path->amount;
	elem_size = 1.0 / node_amount;
	node_time = (time - (elem_size * id.x)) / elem_size;
	cam->pos = vec_bezier_middle(path->pos[id.x].pos, path->pos[id.y].pos,
			path->pos[id.z].pos, node_time);
	cam->look_side = bezier_float_middle(path->pos[id.x].look_side,
			path->pos[id.y].look_side, path->pos[id.z].look_side, node_time);
	cam->look_up = bezier_float_middle(path->pos[id.x].look_up,
			path->pos[id.y].look_up, path->pos[id.z].look_up, node_time);
}

void	camera_path_delete_pos(t_camera_path *path, int index)
{
	while (index < path->amount - 1)
	{
		path->pos[index] = path->pos[index + 1];
		index++;
	}
	path->pos = (t_player_pos*)ft_realloc(path->pos,
		sizeof(t_player_pos) * path->amount, sizeof(t_player_pos) * (path->amount - 1));
	path->amount--;
}

void	camera_path_add_pos(t_camera_path *path, t_camera c)
{
	path->pos = (t_player_pos*)ft_realloc(path->pos,
		sizeof(t_player_pos) * path->amount, sizeof(t_player_pos) * (path->amount + 1));
	path->pos[path->amount].look_side = c.look_side;
	path->pos[path->amount].look_up = c.look_up;
	path->pos[path->amount].pos = c.pos;
	path->amount++;
}

void	camera_path_set(t_camera_path *path, t_camera *cam)
{
	float	time;

	ft_memset(cam, 0, sizeof(t_camera));
	if (path->amount == 0)
		return ;
	if (path->amount == 1)
	{
		cam->look_side = path->pos[0].look_side;
		cam->look_up = path->pos[0].look_up;
		cam->pos = path->pos[0].pos;
		return ;
	}
	time = (SDL_GetTicks() - path->start_time)
		/ (1000.0 * path->duration);
	if (time > 2)
	{
		path->start_time = SDL_GetTicks();
		time = 0;
	}
	else if (time > 1)
	{
		if (path->loop)
		{
			path->start_time = SDL_GetTicks();
			time = 0;
		}
		else
			time = 1 - (time - 1);
	}
	if (path->amount == 2)
		cam_lerp(cam, path->pos[0], path->pos[1], time);
	else
		camera_path_get_pos_bezier(path, cam, time);
}
