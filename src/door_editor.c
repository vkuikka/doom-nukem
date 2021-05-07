/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   door_editor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 23:29:24 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/06 23:29:24 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	find_selected_door_index(t_level *level)
{
	//check if selected in a door
	//deselect everything
	//select tris in door
}

void	door_activate(t_level *level)
{
	//find closed door
	//if closest distance < DOOR_ACTIVATION_DISTANCE
}

void	door_animate(t_level *level)
{
	//interpolate
	//t_door* door = level->doors.door[i]
}

void	add_new_door(t_level *level)
{
	//malloc
	//realloc
	//bzero
}

void	set_door_pos_1(t_level *level)
{
	//t_door* door = level->doors.door[i]
}

void	set_door_pos_2(t_level *level)
{
	//t_door* door = level->doors.door[i]
}

void	enable_door_editor(t_level *level)
{
	level->ui.state.ui_location = UI_LOCATION_DOOR_EDITOR;
	level->ui.wireframe = TRUE;
	level->ui.wireframe_on_top = TRUE;
	level->ui.wireframe_culling_visual = FALSE;
	level->ui.vertex_select_mode = FALSE;
}
