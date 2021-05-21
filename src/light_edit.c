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
		set_text_color(LIGHT_LOCATION_INFO_COLOR);
		text_pos.x = pos.x;
		text_pos.y = pos.y;
		render_text("light", window, &text_pos, NULL);
	}
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
