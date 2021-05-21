/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light_edit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 20:19:04 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/21 20:19:04 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	light_put_text(t_window *window, t_level *level)
{
	t_ivec2	text_pos;
	t_vec3	pos;

	for (int i = 0; i < level->light_amount; i++)
	{
		pos = level->lights[i].pos;
		camera_offset(&pos, &level->cam);
		if (pos.z < 0)
			continue ;
		text_pos.x = pos.x;
		text_pos.y = pos.y;
		if (level->selected_light_index && i + 1 == level->selected_light_index)
		{
			set_text_color(WF_SELECTED_COL);
			render_text("-light-", window, &text_pos, NULL);
		}
		else
		{
			set_text_color(LIGHT_LOCATION_INFO_COLOR);
			render_text("light", window, &text_pos, NULL);
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

	level->selected_light_index = 0;
	nearest_len = -1;
	nearest_index = -1;
	for (int i = 0; i < level->light_amount; i++)
	{
		vert = level->lights[i].pos;
		camera_offset(&vert, &level->cam);
		if (vert.z < 0 || vert.x < 0 || vert.x > RES_X || vert.y < 0 || vert.y > RES_Y)
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

void	delete_light(t_level *level)
{
	if (level->light_amount < 1)
		return ;
	for (int i = level->selected_light_index - 1; i < level->light_amount - 1; i++)
		level->lights[i] = level->lights[i + 1];
	level->light_amount--;
	if (level->light_amount < 1)
	{
		free(level->lights);
		level->lights = NULL;
	}
	else
	{
		if (!(level->lights = (t_light*)realloc(level->lights, sizeof(t_light) * level->light_amount)))
			ft_error("memory allocation failed\n");
	}
	level->selected_light_index = 0;
}

void	add_light(t_level *level)
{
	level->light_amount++;
    if (!(level->lights = (t_light*)realloc(level->lights, sizeof(t_light) * level->light_amount)))
		ft_error("memory allocation failed\n");
	vec_add(&level->lights[level->light_amount - 1].pos, level->cam.pos, level->cam.front);
	level->lights[level->light_amount - 1].brightness = 1;
	level->lights[level->light_amount - 1].radius = 10;
}

void	enable_light_editor(t_level *level)
{
	level->ui.state.ui_location = UI_LOCATION_LIGHT_EDITOR;
	level->ui.vertex_select_mode = FALSE;
	deselect_all_faces(level);
}
