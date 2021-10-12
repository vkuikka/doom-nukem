/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_editor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/22 23:13:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/10/12 13:57:49 by rpehkone         ###   ########.fr       */
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

static void	obj_editor_input_move(t_level *level, t_vec3 move_amount,
			t_vec3 avg, int selected_vert_amount)
{
	level->ui.state.gizmo_active = TRUE;
	vec_div(&avg, selected_vert_amount);
	level->ui.state.gizmo_pos = avg;
	move_selected(level, move_amount);
	if (!vec_cmp(move_amount, (t_vec3){0, 0, 0}))
	{
		static_culling(level);
		level->bake_status = BAKE_NOT_BAKED;
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
	level->ui.state.gizmo_active = FALSE;
	if (selected_vert_amount)
		obj_editor_input_move(level, move_amount, avg, selected_vert_amount);
}

void	set_new_face_pos(t_tri *tri, t_vec3 avg, float scale)
{
	tri->verts[0].txtr.x = 0.;
	tri->verts[0].txtr.y = 0.;
	tri->verts[1].txtr.x = 0.;
	tri->verts[1].txtr.y = 1.;
	tri->verts[2].txtr.x = 1.;
	tri->verts[2].txtr.y = 0.;
	tri->verts[3].txtr.x = 1.;
	tri->verts[3].txtr.y = 1.;
	tri->verts[0].pos = avg;
	tri->verts[1].pos = avg;
	tri->verts[2].pos = avg;
	tri->verts[3].pos = avg;
	tri->verts[0].pos.x -= 2 * scale;
	tri->verts[0].pos.y += 2 * scale;
	tri->verts[1].pos.x -= 2 * scale;
	tri->verts[1].pos.y -= 2 * scale;
	tri->verts[2].pos.x += 2 * scale;
	tri->verts[2].pos.y += 2 * scale;
	tri->verts[3].pos.x += 2 * scale;
	tri->verts[3].pos.y -= 2 * scale;
}

void	set_new_face(t_tri *tri, t_vec3 pos, t_vec3 dir, float scale)
{
	t_vec3	tri_avg;

	ft_bzero(tri, sizeof(t_tri));
	tri_avg = dir;
	tri->isquad = TRUE;
	vec_mult(&tri_avg, 2);
	vec_add(&tri_avg, pos, tri_avg);
	set_new_face_pos(tri, tri_avg, scale);
	vec_sub(&tri->v0v2, tri->verts[1].pos, tri->verts[0].pos);
	vec_sub(&tri->v0v1, tri->verts[2].pos, tri->verts[0].pos);
	vec_cross(&tri->normal, tri->v0v2, tri->v0v1);
	vec_normalize(&tri->normal);
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
	set_new_face(&level->all.tris[level->all.tri_amount - 1],
		level->cam.pos, level->cam.front, 1);
	level->all.tris[level->all.tri_amount - 1].index
		= level->all.tri_amount - 1;
	init_screen_space_partition(level);
	init_culling(level);
}

void	remove_tri(t_level *level, int i)
{
	int	k;

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
