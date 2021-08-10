/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_quads.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/10 20:29:25 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

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

void	set_mirror_dir(t_tri *a, int not_shared_index)
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
	vec_sub(&a->v0v2, a->verts[1].pos, a->verts[0].pos);
	vec_sub(&a->v0v1, a->verts[2].pos, a->verts[0].pos);
}

int	has_2_shared_vertices(t_tri a, t_tri b, int *not_shared)
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
		j = 0;
		while (j < 3)
		{
			if (vec_cmp(a.verts[i].pos, b.verts[j].pos))
			{
				found = 1;
				shared_count++;
			}
			j++;
		}
		if (!found)
			(*not_shared) = i;
		i++;
	}
	return (shared_count == 2);
}

int	is_mirror(t_tri a, t_tri b, int *not_shared_index)
{
	int		not_shared[2];
	int		x;
	int		y;
	t_vec3	shared1;
	t_vec3	shared2;
	t_vec3	nshared;
	t_vec3	avg;
	t_vec3	res;
	t_vec3	aa;
	float	len;

	if (!has_2_shared_vertices(a, b, &not_shared[0]))
		return (0);
	has_2_shared_vertices(b, a, &not_shared[1]);
	x = 0;
	y = 1;
	if (not_shared[0] == 0)
	{
		x++;
		y++;
	}
	else if (not_shared[0] == 1)
		y++;
	vec_sub(&shared1, a.verts[x].pos, a.verts[not_shared[0]].pos);
	vec_sub(&shared2, a.verts[y].pos, a.verts[not_shared[0]].pos);
	vec_sub(&nshared, b.verts[not_shared[1]].pos, a.verts[not_shared[0]].pos);
	vec_avg(&avg, shared1, shared2);
	vec_add(&res, avg, avg);
	(*not_shared_index) = not_shared[0];
	vec_sub(&aa, res, nshared);
	len = vec_length(aa);
	return ((len < 0.0001));
	return (vec_cmp(res, nshared));
}

void	find_quads(t_obj *obj)
{
	int	tris;
	int	not_shared_index;
	int	i;
	int	j;

	tris = obj->tri_amount;
	i = -1;
	while (++i < obj->tri_amount)
	{
		j = -1;
		while (++j < obj->tri_amount)
		{
			if (i != j && !obj->tris[i].isquad && !obj->tris[j].isquad)
			{
				if (is_mirror(obj->tris[i], obj->tris[j], &not_shared_index))
				{
					set_mirror_dir(&obj->tris[i], not_shared_index);
					set_fourth_vertex(&obj->tris[i]);
					set_fourth_vertex_uv(&obj->tris[i]);
					obj->tris[i].isquad = 1;
					obj->tri_amount--;
					if (j < i)
						i--;
					j--;
					while (++j < obj->tri_amount)
						obj->tris[j] = obj->tris[j + 1];
					j = 0;
				}
			}
		}
	}
	obj->tris = (t_tri *)ft_realloc(obj->tris, sizeof(t_tri) * tris,
			sizeof(t_tri) * obj->tri_amount);
	if (!obj->tris)
		ft_error("memory allocation failed\n");
}
