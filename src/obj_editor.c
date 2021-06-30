/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_editor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/22 23:13:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/19 17:07:38y vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void			transform_quad(t_tri *tri, t_vec3 dir)
{
	for (int k = 0; k < 3 + tri->isquad; k++)
	{
		int quadmoved = 0;
		if (k == 3 && quadmoved)
			return ;
		if (tri->verts[k].selected)
		{
			vec_add(&tri->verts[k].pos, tri->verts[k].pos, dir);
			if (k < 3 && tri->isquad)
			{
				set_fourth_vertex(tri);
				quadmoved = 1;
			}
			else if (k == 3)
			{
				tri->verts[0].pos = tri->verts[3].pos;
				set_fourth_vertex(tri);
				t_vec3 tmp = tri->verts[0].pos;
				tri->verts[0].pos = tri->verts[3].pos;
				tri->verts[3].pos = tmp;
			}
		}
	}
}

void			move_selected(t_level *level, t_vec3 dir)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		int amount = 0;
		for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
			if (level->all.tris[i].verts[k].selected)
				amount++;
		if (level->ui.vertex_select_mode && level->all.tris[i].isquad && amount != 4)
			transform_quad(&level->all.tris[i], dir);
		else
		{
			for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
				if (level->all.tris[i].selected || (level->ui.vertex_select_mode && level->all.tris[i].verts[k].selected))
				vec_add(&level->all.tris[i].verts[k].pos, level->all.tris[i].verts[k].pos, dir);
		}
		vec_sub(&level->all.tris[i].v0v2, level->all.tris[i].verts[1].pos, level->all.tris[i].verts[0].pos);
		vec_sub(&level->all.tris[i].v0v1, level->all.tris[i].verts[2].pos, level->all.tris[i].verts[0].pos);
	}
}

void				obj_editor_input(t_level *level, t_vec3 move_amount)
{
	t_vec3	avg = {0, 0, 0};
	int		selected_vert_amount;

	selected_vert_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
		for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
			if (level->all.tris[i].selected || (level->ui.vertex_select_mode && level->all.tris[i].verts[k].selected))
			{
				vec_add(&avg, avg, level->all.tris[i].verts[k].pos);
				selected_vert_amount++;
			}
	vec_div(&avg, selected_vert_amount);
	level->ui.state.gizmo_pos = avg;
	move_selected(level, move_amount);
}

void			set_new_face_pos(t_obj *obj, int i, t_vec3 avg, float scale)
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

void			set_new_face(t_level *level, t_vec3 pos, t_vec3 dir, float scale)
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
	vec_sub(&level->all.tris[i].v0v2, level->all.tris[i].verts[1].pos, level->all.tris[i].verts[0].pos);
	vec_sub(&level->all.tris[i].v0v1, level->all.tris[i].verts[2].pos, level->all.tris[i].verts[0].pos);
	vec_cross(&level->all.tris[i].normal, level->all.tris[i].v0v2, level->all.tris[i].v0v1);
	vec_normalize(&level->all.tris[i].normal);
}

void			add_face(t_level *level)
{
	free_culling(level);
	level->all.tri_amount++;
	if (!(level->all.tris = (t_tri*)ft_realloc(level->all.tris, sizeof(t_tri) * level->all.tri_amount - 1, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	if (!(level->visible.tris = (t_tri*)ft_realloc(level->visible.tris, sizeof(t_tri) * level->all.tri_amount - 1, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	set_new_face(level, level->cam.pos, level->cam.front, 1);
	init_screen_space_partition(level);
	init_culling(level);
}

void			remove_faces(t_level *level)
{
	int	amount;
	int	original_amount;

	free_culling(level);
	amount = level->all.tri_amount;
	original_amount = level->all.tri_amount;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].selected)
		{
			if (level->all.tris[i].enemy)
				free(level->all.tris[i].enemy);
			if (level->all.tris[i].projectile)
				free(level->all.tris[i].projectile);
			for (int k = i; k < level->all.tri_amount - 1; k++)
			{
				level->all.tris[k] = level->all.tris[k + 1];
				level->all.tris[k].index = k;
			}
			amount--;
		}
	}
	level->all.tri_amount = amount;
	if (!(level->all.tris = (t_tri*)ft_realloc(level->all.tris, sizeof(t_tri) * original_amount, sizeof(t_tri) * amount)))
		ft_error("memory allocation failed");
	if (!(level->visible.tris = (t_tri*)ft_realloc(level->visible.tris, sizeof(t_tri) * original_amount, sizeof(t_tri) * amount)))
		ft_error("memory allocation failed");
	init_screen_space_partition(level);
	init_culling(level);
}
