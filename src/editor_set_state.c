/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   editor_set_state.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/11 15:51:06 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/02 16:25:39 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	set_obj(t_level *level, char *filename)
{
	t_obj	tmp;

	if (!load_obj(filename, &tmp))
	{
		nonfatal_error(level, "failed to read object file");
		return ;
	}
	free_culling(level);
	free(level->all.tris);
	free(level->visible.tris);
	level->all = tmp;
	level->visible.tris
		= (t_tri *)malloc(sizeof(t_tri) * level->all.tri_amount);
	if (!level->visible.tris)
		ft_error("memory allocation failed\n");
	init_screen_space_partition(level);
	init_culling(level);
}

void	set_texture(t_level *level, char *filename)
{
	free(level->texture.image);
	free(level->baked);
	free(level->spray_overlay);
	level->texture = bmp_read(filename);
	level->baked = (t_color *)malloc(sizeof(t_color)
			* (level->texture.width * level->texture.height));
	level->spray_overlay = (unsigned int *)malloc(sizeof(unsigned int)
			* (level->texture.width * level->texture.height));
	if (!level->spray_overlay || !level->baked)
		ft_error("memory allocation failed\n");
	ft_bzero(level->spray_overlay,
		level->texture.width * level->texture.height * 4);
}

void	set_normal_map(t_level *level, char *filename)
{
	free(level->normal_map.image);
	level->normal_map = bmp_read(filename);
}

void	set_skybox(t_level *level, char *filename)
{
	free(level->sky.img.image);
	level->sky.img = bmp_read(filename);
}

void	set_spray(t_level *level, char *filename)
{
	free(level->spray.image);
	level->spray = bmp_read(filename);
}

void	set_win_pos(t_level *level)
{
	level->win_pos = level->cam.pos;
}

void	set_spawn_pos(t_level *level)
{
	level->spawn_pos.pos = level->cam.pos;
	level->spawn_pos.look_side = level->cam.look_side;
	level->spawn_pos.look_up = level->cam.look_up;
}

void	set_menu_pos_1(t_level *level)
{
	level->main_menu_pos1.pos = level->cam.pos;
	level->main_menu_pos1.look_side = level->cam.look_side;
	level->main_menu_pos1.look_up = level->cam.look_up;
}

void	set_menu_pos_2(t_level *level)
{
	level->main_menu_pos2.pos = level->cam.pos;
	level->main_menu_pos2.look_side = level->cam.look_side;
	level->main_menu_pos2.look_up = level->cam.look_up;
}
