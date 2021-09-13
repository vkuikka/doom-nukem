/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_quads.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/02 10:21:54 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

/*
 * Purpose of file: Iterate 3D object and find all pairs of perfectly
 * mirrored triangles and convert those to quads, because we can
 * raycast the same surface area with no additional cost.
 * Faces found here get an isquad property (t_tri.isquad)
 */

void	set_fourth_vertex(t_tri *a)
{
	t_vec3	shared1;
	t_vec3	shared2;
	t_vec3	avg;
	t_vec3	new;
	t_vec3	res;

	vec_sub(&shared1, a->verts[1].pos, a->verts[0].pos);
	vec_sub(&shared2, a->verts[2].pos, a->verts[0].pos);
	vec_avg(&avg, shared1, shared2);
	vec_add(&new, avg, avg);
	vec_add(&res, new, a->verts[0].pos);
	a->verts[3].pos.x = res.x;
	a->verts[3].pos.y = res.y;
	a->verts[3].pos.z = res.z;
}

static void	set_mirrored_dir(t_tri *a, int not_shared_index)
{
	t_vec3	tmp;
	t_vec2	v2tmp;

	if (not_shared_index == 1)
	{
		tmp = a->verts[0].pos;
		a->verts[0].pos = a->verts[1].pos;
		a->verts[1].pos = a->verts[2].pos;
		a->verts[2].pos = tmp;
		v2tmp = a->verts[0].txtr;
		a->verts[0].txtr = a->verts[1].txtr;
		a->verts[1].txtr = a->verts[2].txtr;
		a->verts[2].txtr = v2tmp;
	}
	else if (not_shared_index == 2)
	{
		tmp = a->verts[0].pos;
		a->verts[0].pos = a->verts[2].pos;
		a->verts[2].pos = a->verts[1].pos;
		a->verts[1].pos = tmp;
		v2tmp = a->verts[0].txtr;
		a->verts[0].txtr = a->verts[2].txtr;
		a->verts[2].txtr = a->verts[1].txtr;
		a->verts[1].txtr = v2tmp;
	}
}

static int	has_2_shared_vertices(t_tri a, t_tri b, int *not_shared)
{
	int	shared_count;
	int	found;
	int	i;
	int	j;

	shared_count = 0;
	(*not_shared) = 2;
	i = 0;
	while (i < 3)
	{
		found = 0;
		j = -1;
		while (++j < 3)
		{
			if (vec_cmp(a.verts[i].pos, b.verts[j].pos))
			{
				found = 1;
				shared_count++;
			}
		}
		if (!found)
			(*not_shared) = i;
		i++;
	}
	return (shared_count == 2);
}

static int	is_mirrored_set(t_tri *a, t_tri *b)
{
	int		not_shared_index;
	t_vec3	test;
	float	len;

	if (!has_2_shared_vertices(*a, *b, &not_shared_index))
		return (0);
	set_mirrored_dir(a, not_shared_index);
	has_2_shared_vertices(*b, *a, &not_shared_index);
	set_mirrored_dir(b, not_shared_index);
	set_fourth_vertex(a);
	vec_sub(&test, a->verts[3].pos, b->verts[0].pos);
	len = vec_length(test);
	if (len < 0)
		len *= -1;
	if (len > FIND_QUADS_FLOAT_ERROR)
		return (0);
	a->verts[3].txtr = b->verts[0].txtr;
	a->isquad = 1;
	return (1);
}

void	find_quads(t_obj *obj)
{
	int	prev_tri_amount;
	int	i;
	int	j;

	prev_tri_amount = obj->tri_amount;
	i = -1;
	while (++i < obj->tri_amount)
	{
		j = i;
		while (++j < obj->tri_amount)
		{
			if (!obj->tris[i].isquad && !obj->tris[j].isquad
				&& is_mirrored_set(&obj->tris[i], &obj->tris[j]))
			{
				obj->tri_amount--;
				j--;
				while (++j < obj->tri_amount)
					obj->tris[j] = obj->tris[j + 1];
			}
		}
	}
	obj->tris = (t_tri *)ft_realloc(obj->tris, sizeof(t_tri) * prev_tri_amount,
			sizeof(t_tri) * obj->tri_amount);
	if (!obj->tris)
		ft_error("memory allocation failed\n");
}
