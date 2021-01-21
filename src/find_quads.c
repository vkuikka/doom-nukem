/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_quads.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/21 19:19:07 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	set_mirror_dir(t_tri *a, int not_shared[2])
{
	float tmp[3];

	if (not_shared[0] == 1)
	{
		vec_copy(tmp, a->verts[0].pos);
		vec_copy(a->verts[0].pos, a->verts[1].pos);
		vec_copy(a->verts[1].pos, a->verts[2].pos);
		vec_copy(a->verts[2].pos, tmp);
	}
	else if (not_shared[0] == 2)
	{
		vec_copy(tmp, a->verts[0].pos);
		vec_copy(a->verts[0].pos, a->verts[2].pos);
		vec_copy(a->verts[2].pos, a->verts[1].pos);
		vec_copy(a->verts[1].pos, tmp);
	}
}

int		is_mirror(t_tri a, t_tri b, int not_shared[2])
{

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
	return (vec_cmp(res, nshared));
}

int		has_2_shared_vertices(t_tri a, t_tri b, int *not_shared)
{
	int	shared_count;
	int found;

	shared_count = 0;
	(*not_shared) = 2;
	for (int i = 0; i < 3; i++)
	{
		found = 0;
		for (int j = 0; j < 3; j++)
		{
			if (vec_cmp(a.verts[i].pos, b.verts[j].pos))
			{
				found = 1;
				shared_count++;
			}
		}
		if (!found)
			(*not_shared) = i;
	}
	return (shared_count == 2);
}

void	find_quads(t_obj *obj)
{
	int quad = 0;

	for (int i = 0; i < obj->tri_amount; i++)
	{
		for (int j = 0; j < obj->tri_amount; j++)
		{
			if (i != j && !obj->tris[i].isquad && !obj->tris[j].isquad)
			{
				int not_shared[2];
				if (has_2_shared_vertices(obj->tris[i], obj->tris[j], &not_shared[0]))
				{
					has_2_shared_vertices(obj->tris[j], obj->tris[i], &not_shared[1]);
					if (is_mirror(obj->tris[i], obj->tris[j], not_shared))
					{
						quad++;
						set_mirror_dir(&obj->tris[i], not_shared);
						vec_sub(obj->tris[i].v0v2, obj->tris[i].verts[1].pos, obj->tris[i].verts[0].pos);
						vec_sub(obj->tris[i].v0v1, obj->tris[i].verts[2].pos, obj->tris[i].verts[0].pos);
						obj->tris[i].isquad = 1;
						//free(obj->tris[j]); //delete other part of quad //cant delete only one // maybe calloc?
						obj->tri_amount--;
						for (int x = j; x < obj->tri_amount; x++) //move address of everythign left
							obj->tris[x] = obj->tris[x + 1];
						i = 0;
						j = 0;
					}
				}
			}
		}
	}
	printf("quads = %d\n", quad);
	//realloc(obj->tris, sizeof(t_tri) * obj->tri_amount)
}
