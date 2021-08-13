/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light_edit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 20:19:04 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/13 17:18:27 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	light_put_text(t_window *window, t_level *level)
{
	t_vec3	pos;
	int		i;

	i = -1;
	while (++i < level->light_amount)
	{
		pos = level->lights[i].pos;
		camera_offset(&pos, &level->cam);
		if (pos.z < 0)
			continue ;
		if (level->selected_light_index && i + 1 == level->selected_light_index)
		{
			set_text_color(WF_SELECTED_COL);
			render_text("-light-", pos.x, pos.y);
		}
		else
		{
			set_text_color(LIGHT_LOCATION_INFO_COLOR);
			render_text("light", pos.x, pos.y);
		}
	}
}

void	select_light(t_level *level, int x, int y)
{
	float	nearest_len;
	int		nearest_index;
	float	len;
	t_vec2	test;
	t_vec3	vert;
	int		i;

	level->selected_light_index = 0;
	nearest_len = -1;
	nearest_index = -1;
	i = -1;
	while (++i < level->light_amount)
	{
		vert = level->lights[i].pos;
		camera_offset(&vert, &level->cam);
		if (vert.z < 0 || vert.x < 0 || vert.x > RES_X || vert.y < 0
			|| vert.y > RES_Y)
			continue ;
		test.x = vert.x;
		test.y = vert.y;
		test.x -= x;
		test.y -= y;
		len = vec2_length(test);
		if (len < nearest_len || nearest_len == -1)
		{
			nearest_len = len;
			nearest_index = i;
		}
	}
	if (nearest_index != -1)
		level->selected_light_index = nearest_index + 1;
}

void	move_light(t_level *level, t_vec3 move_amount)
{
	if (level->selected_light_index)
	{
		vec_add(&level->lights[level->selected_light_index - 1].pos,
			level->lights[level->selected_light_index - 1].pos,
			move_amount);
		level->ui.state.gizmo_pos
			= level->lights[level->selected_light_index - 1].pos;
	}
}

void	delete_light(t_level *level)
{
	int	i;

	if (level->light_amount < 1)
		return ;
	i = level->selected_light_index - 2;
	while (++i < level->light_amount - 1)
		level->lights[i] = level->lights[i + 1];
	level->light_amount--;
	if (level->light_amount < 1)
	{
		free(level->lights);
		level->lights = NULL;
	}
	else
	{
		level->lights = (t_light *)ft_realloc(level->lights,
				sizeof(t_light) * level->light_amount + 1,
				sizeof(t_light) * level->light_amount);
		if (!level->lights)
			ft_error("memory allocation failed\n");
	}
	level->selected_light_index = 0;
	level->bake_status = BAKE_NOT_BAKED;
}

void	add_light(t_level *level)
{
	level->light_amount++;
	level->lights = (t_light *)ft_realloc(level->lights,
			sizeof(t_light) * level->light_amount - 1,
			sizeof(t_light) * level->light_amount);
	if (!level->lights)
		ft_error("memory allocation failed\n");
	vec_add(&level->lights[level->light_amount - 1].pos,
		level->cam.pos,
		level->cam.front);
	level->lights[level->light_amount - 1].color.r = 1;
	level->lights[level->light_amount - 1].color.g = 1;
	level->lights[level->light_amount - 1].color.b = 1;
	level->lights[level->light_amount - 1].radius = 10;
	level->bake_status = BAKE_NOT_BAKED;
}

void	enable_light_editor(t_level *level)
{
	level->ui.state.ui_location = UI_LOCATION_LIGHT_EDITOR;
	level->ui.vertex_select_mode = FALSE;
	deselect_all_faces(level);
}
