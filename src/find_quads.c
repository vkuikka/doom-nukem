/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_quads.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/21 20:39:39 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	set_mirror_dir(t_tri *a, int not_shared_vertex_index)
{
	float tmp[3];

	if (not_shared_vertex_index == 1)
	{
		vec_copy(tmp, a->verts[0].pos);
		vec_copy(a->verts[0].pos, a->verts[1].pos);
		vec_copy(a->verts[1].pos, a->verts[2].pos);
		vec_copy(a->verts[2].pos, tmp);
	}
	else if (not_shared_vertex_index == 2)
	{
		vec_copy(tmp, a->verts[0].pos);
		vec_copy(a->verts[0].pos, a->verts[2].pos);
		vec_copy(a->verts[2].pos, a->verts[1].pos);
		vec_copy(a->verts[1].pos, tmp);
	}
	vec_sub(a->v0v2, a->verts[1].pos, a->verts[0].pos);
	vec_sub(a->v0v1, a->verts[2].pos, a->verts[0].pos);
}

int		has_2_shared_vertices(t_tri a, t_tri b, int *not_shared)
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

int		is_mirror(t_tri a, t_tri b, int *not_shared_vertex_index)
{
	int not_shared[2];

	if (!has_2_shared_vertices(a, b, &not_shared[0]))
		return (0);
	has_2_shared_vertices(b, a, &not_shared[1]);

	int x = 0;
	int y = 1;
	if (not_shared[0] == 0)
	{
		x++;
		y++;
	}
	else if (not_shared[0] == 1)
		y++;
	float shared1[3];
	float shared2[3];
	float nshared[3];
	vec_sub(shared1, a.verts[x].pos, a.verts[not_shared[0]].pos);
	vec_sub(shared2, a.verts[y].pos, a.verts[not_shared[0]].pos);
	vec_sub(nshared, b.verts[not_shared[1]].pos, a.verts[not_shared[0]].pos);
	float avg[3];
	float res[3];
	vec_avg(avg, shared1, shared2);
	vec_add(res, avg, avg);
	(*not_shared_vertex_index) = not_shared[0];
	float aa[3];
	vec_sub(aa, res, nshared);
	float len = vec_length(aa);
	return ((len < 0.0001));
	return (vec_cmp(res, nshared));
}

void	find_quads(t_obj *obj)
{
	int quads = 0;

	for (int i = 0; i < obj->tri_amount; i++)
	{
		for (int j = 0; j < obj->tri_amount; j++)
		{
			if (i != j && !obj->tris[i].isquad && !obj->tris[j].isquad)
			{
				int not_shared_vertex_index;;
				if (is_mirror(obj->tris[i], obj->tris[j], &not_shared_vertex_index))
				{
					set_mirror_dir(&obj->tris[i], not_shared_vertex_index);
					obj->tris[i].isquad = 1;
					obj->tri_amount--;
					for (int x = j; x < obj->tri_amount; x++) //move address of everythign left
						obj->tris[x] = obj->tris[x + 1];
					i = 0;
					j = 0;
					quads++;
				}
			}
		}
	}
	if (!(obj->tris = (t_tri*)realloc(obj->tris, sizeof(t_tri) * obj->tri_amount)))
		ft_error("memory allocation failed\n");
	printf("quads = %d\n", quads);
}
