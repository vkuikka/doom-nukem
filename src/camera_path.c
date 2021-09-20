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
	int		node_id;
	float	elem_size;
	float	node_time;

	node_id = (path->amount - 2) * time;
	elem_size = 1.0 / (path->amount - 2);
	node_time = (time - (elem_size * node_id)) / elem_size;
	cam->pos = vec_bezier_middle(path->pos[node_id + 0].pos, path->pos[node_id + 1].pos, path->pos[node_id + 2].pos, node_time);
	cam->look_side = bezier_float_middle(path->pos[node_id + 0].look_side, path->pos[node_id + 1].look_side, path->pos[node_id + 2].look_side, node_time);
	cam->look_up = bezier_float_middle(path->pos[node_id + 0].look_up, path->pos[node_id + 1].look_up, path->pos[node_id + 2].look_up, node_time);
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
	else if (time > 1)//loop button here
		time = 1 - (time - 1);
	if (path->amount == 2)
		cam_lerp(cam, path->pos[0], path->pos[1], time);
	else
		camera_path_get_pos_bezier(path, cam, time);
}
