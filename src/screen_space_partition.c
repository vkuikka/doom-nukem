/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screen_space_partition.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/09 12:03:36 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/09 12:03:36 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static int			is_tri_left(t_tri *tri, float angle, t_vec3 *pos)
{
	t_vec3	dir;

	vec_rot(&dir, (t_vec3){0, 0, 1}, angle - (M_PI / 2));
	return (normal_plane_culling(*tri, pos, &dir));
}

static int			is_tri_right(t_tri *tri, float angle, t_vec3 *pos)
{
	t_vec3	dir;

	vec_rot(&dir, (t_vec3){0, 0, 1}, angle + (M_PI / 2));
	return (normal_plane_culling(*tri, pos, &dir));
}

static void		find_ssp_index(t_tri *tri, t_level *level, t_ivec2 x_range, int depth, float angle)
{
    if (is_tri_left(tri, angle, &level->pos))
    {
        if (SSP_MAX_X / depth == 2)
            level->ssp[x_range.x].tris[level->ssp[x_range.x].tri_amount++] = *tri;
        else
        {
            x_range.y = (x_range.y + 1) / 2 - 1;
            find_ssp_index(tri, level, x_range, depth + 1 , angle - (M_PI / 8));
        }
    }
    else if (is_tri_right(tri, angle, &level->pos))
    {

        if (SSP_MAX_X / depth == 2)
            level->ssp[x_range.y].tris[level->ssp[x_range.y].tri_amount++] = *tri;
        else
        {
            x_range.x += (x_range.y - x_range.x + 1) / 2;
            find_ssp_index(tri, level, x_range, depth + 1 , angle + (M_PI / 8));
        }
    }
    else
    {
        // if (depth == 1 || SSP_MAX_X / depth == 2)
            for (int i = x_range.x; i <= x_range.y; i++)
                level->ssp[i].tris[level->ssp[i].tri_amount++] = *tri;
        // else
            //lis'' check jos se on molemmissa keskell
    }
}

void		screen_space_partition(t_level *level)
{
	for (int i = 0; i < SSP_MAX_X; i++)
		level->ssp[i].tri_amount =  0;
	for (int i = 0; i < level->visible.tri_amount; i++)
	{
		if (level->visible.tris[i].isgrid)
		{
			for (int o = 0; o < SSP_MAX_X; o++)
				level->ssp[o].tris[level->ssp[o].tri_amount++] = level->visible.tris[i];
		}
		else
			find_ssp_index(&level->visible.tris[i], level, (t_ivec2){0, SSP_MAX_X - 1}, 1, level->look_side);
	}
}

int			get_ssp_index(int x, int y)
{
	for (int i = 0; i < SSP_MAX_X; i++)
		if (RES_X / SSP_MAX_X * i <= x && RES_X / SSP_MAX_X * (i + 1) > x)
			return (i);
	ft_error("ssp pixel out of screen");
	return (0);
}

void		init_screen_space_partition(t_level *level)
{
	if (!(level->ssp = (t_obj*)malloc(sizeof(t_obj) * SSP_MAX_X)))
		ft_error("memory allocation failed");
	for (int i = 0; i < SSP_MAX_X; i++)
		if (!(level->ssp[i].tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
			ft_error("memory allocation failed");
}