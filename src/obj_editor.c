/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_editor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/22 23:13:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/03 07:37:02 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	transform_quad(t_tri *tri, t_vec3 dir)
{
	t_vec3	tmp;
	int		k;

	k = -1;
	while (++k < 3 + tri->isquad)
	{
		if (tri->verts[k].selected)
		{
			vec_add(&tri->verts[k].pos, tri->verts[k].pos, dir);
			if (k < 3 && tri->isquad)
			{
				set_fourth_vertex(tri);
				if (k == 2)
					return ;
			}
			else if (k == 3)
			{
				tri->verts[0].pos = tri->verts[3].pos;
				set_fourth_vertex(tri);
				tmp = tri->verts[0].pos;
				tri->verts[0].pos = tri->verts[3].pos;
				tri->verts[3].pos = tmp;
			}
		}
	}
}

void	tri_optimize(t_tri *tri)
{
	vec_sub(&tri->v0v2, tri->verts[1].pos, tri->verts[0].pos);
	vec_sub(&tri->v0v1, tri->verts[2].pos, tri->verts[0].pos);
}

void	move_selected(t_level *l, t_vec3 dir)
{
	int	amount;
	int	i;
	int	k;

	i = -1;
	while (++i < l->all.tri_amount)
	{
		amount = 0;
		k = -1;
		while (++k < 3 + l->all.tris[i].isquad)
			if (l->all.tris[i].verts[k].selected)
				amount++;
		if (l->ui.vertex_select_mode && l->all.tris[i].isquad && amount != 4)
			transform_quad(&l->all.tris[i], dir);
		else
		{
			k = -1;
			while (++k < 3 + l->all.tris[i].isquad)
				if (l->all.tris[i].selected || (l->ui.vertex_select_mode
						&& l->all.tris[i].verts[k].selected))
					vec_add(&l->all.tris[i].verts[k].pos,
						l->all.tris[i].verts[k].pos, dir);
		}
		tri_optimize(&l->all.tris[i]);
	}
}

void	obj_editor_input(t_level *level, t_vec3 move_amount)
{
	t_vec3	avg;
	int		selected_vert_amount;
	int		i;
	int		k;

	avg = (t_vec3){0, 0, 0};
	selected_vert_amount = 0;
	i = -1;
	while (++i < level->all.tri_amount)
	{
		k = -1;
		while (++k < 3 + level->all.tris[i].isquad)
		{
			if (level->all.tris[i].selected
				|| (level->ui.vertex_select_mode
					&& level->all.tris[i].verts[k].selected))
			{
				vec_add(&avg, avg, level->all.tris[i].verts[k].pos);
				selected_vert_amount++;
			}
		}
	}
	vec_div(&avg, selected_vert_amount);
	level->ui.state.gizmo_pos = avg;
	move_selected(level, move_amount);
}

void	set_new_face_pos(t_obj *obj, int i, t_vec3 avg, float scale)
{
	obj->tris[i].verts[0].txtr.x = .5;
	obj->tris[i].verts[0].txtr.y = 0.;
	obj->tris[i].verts[1].txtr.x = 0.;
	obj->tris[i].verts[1].txtr.y = 1.;
	obj->tris[i].verts[2].txtr.x = 1.;
	obj->tris[i].verts[2].txtr.y = 1.;
	obj->tris[i].verts[0].pos = avg;
	obj->tris[i].verts[1].pos = avg;
	obj->tris[i].verts[2].pos = avg;
	obj->tris[i].verts[0].pos.y -= 2 * scale;
	obj->tris[i].verts[1].pos.y += 2 * scale;
	obj->tris[i].verts[2].pos.y += 2 * scale;
	obj->tris[i].verts[1].pos.x -= 2 * scale;
	obj->tris[i].verts[2].pos.x += 2 * scale;
}

void	set_new_face(t_level *level, t_vec3 pos, t_vec3 dir, float scale)
{
	int		i;
	t_vec3	tri_avg;

	i = level->all.tri_amount - 1;
	ft_bzero(&level->all.tris[i], sizeof(t_tri));
	level->all.tris[i].index = i;
	tri_avg = dir;
	vec_mult(&tri_avg, 2);
	vec_add(&tri_avg, pos, tri_avg);
	set_new_face_pos(&level->all, i, tri_avg, scale);
	vec_sub(&level->all.tris[i].v0v2,
		level->all.tris[i].verts[1].pos, level->all.tris[i].verts[0].pos);
	vec_sub(&level->all.tris[i].v0v1,
		level->all.tris[i].verts[2].pos, level->all.tris[i].verts[0].pos);
	vec_cross(&level->all.tris[i].normal,
		level->all.tris[i].v0v2, level->all.tris[i].v0v1);
	vec_normalize(&level->all.tris[i].normal);
}

void	add_face(t_level *level)
{
	free_culling(level);
	level->all.tri_amount++;
	level->all.tris = (t_tri *)ft_realloc(level->all.tris,
			sizeof(t_tri) * level->all.tri_amount - 1,
			sizeof(t_tri) * level->all.tri_amount);
	if (!level->all.tris)
		ft_error("memory allocation failed");
	level->visible.tris = (t_tri *)ft_realloc(level->visible.tris,
			sizeof(t_tri) * level->all.tri_amount - 1,
			sizeof(t_tri) * level->all.tri_amount);
	if (!level->visible.tris)
		ft_error("memory allocation failed");
	set_new_face(level, level->cam.pos, level->cam.front, 1);
	init_screen_space_partition(level);
	init_culling(level);
}

void	remove_tri(t_level *level, int i)
{
	int	k;

	if (level->all.tris[i].enemy)
		free(level->all.tris[i].enemy);
	if (level->all.tris[i].projectile)
		free(level->all.tris[i].projectile);
	k = i - 1;
	while (++k < level->all.tri_amount - 1)
	{
		level->all.tris[k] = level->all.tris[k + 1];
		level->all.tris[k].index = k;
	}
}

void	remove_faces(t_level *level)
{
	int	amount;
	int	original_amount;
	int	i;

	free_culling(level);
	amount = level->all.tri_amount;
	original_amount = level->all.tri_amount;
	i = -1;
	while (++i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
			remove_tri(level, i);
		if (level->all.tris[i].selected)
			amount--;
	}
	level->all.tri_amount = amount;
	level->all.tris = (t_tri *)ft_realloc(level->all.tris,
			sizeof(t_tri) * original_amount, sizeof(t_tri) * amount);
	level->visible.tris = (t_tri *)ft_realloc(level->visible.tris,
			sizeof(t_tri) * original_amount, sizeof(t_tri) * amount);
	if (!level->visible.tris || !level->all.tris)
		ft_error("memory allocation failed");
	init_screen_space_partition(level);
	init_culling(level);
}
