/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/20 19:19:07 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static char	**file2d(char *filename)
{
	char	**file;
	char	*line;
	int		fd;
	int		i = 0;

	file = (char **)malloc(sizeof(char*) * 10000);
	fd = open(filename, O_RDONLY);
	while (get_next_line(fd, &line))
	{
		//printf("%s\n", line);
		file[i] = line;
		i++;
	}
	file[i] = NULL;
	close(fd);
	/*i = 0;
	while (file[i])
	{
		//file[i] = ft_strsplit(&line[i], ' ');
		printf("%s\n", file[i][0]);
		i++;
	}*/
	return (file);
}

static void	set_uv_vert(char **file, int i, t_vec2 *vert)
{
	int j = 0;

	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->x = ft_atof(&file[i][j]);
	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->y = ft_atof(&file[i][j]);
}

static t_vec2	*load_uvs(char **file)
{
	int i = 0;
	int uv_amount = 0;
	t_vec2 *res = NULL;

	while (file[i])
	{
		if (!ft_strncmp(file[i], "vt ", 3))
			uv_amount++;
		i++;
	}
	if (!(res = (t_vec2*)malloc(sizeof(t_vec2) * uv_amount)))
		ft_error("memory allocation failed\n");
	i = 0;
	int j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "vt ", 3))
		{
			set_uv_vert(file, i, &res[j]);
			j++;
		}
		i++;
	}
	return (res);
}

static void	set_vert(char **file, int i, t_vec3 *vert)
{
	int j = 0;

	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->x = ft_atof(&file[i][j]);
	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->y = ft_atof(&file[i][j]);
	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->z = ft_atof(&file[i][j]);
}

static t_vec3	*load_verts(char **file)
{
	int i = 0;
	int ver_amount = 0;
	t_vec3 *res;

	while (file[i])
	{
		if (!ft_strncmp(file[i], "v ", 2))
			ver_amount++;
		i++;
	}
	if (!(res = (t_vec3*)malloc(sizeof(t_vec3) * ver_amount)))
		ft_error("memory allocation failed\n");
	i = 0;
	int j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "v ", 2))
		{
			set_vert(file, i, &res[j]);
			j++;
		}
		i++;
	}
	/*for (int x = 0; x < ver_amount; x++)
	{
		printf("%d = %f\n", x, res[x].x);
		printf("%d = %f\n", x, res[x].y);
		printf("%d = %f\n", x, res[x].z);
	}*/
	return (res);
}

static void	set_tri(char *str, t_vec3 *verts, t_vec2 *uvs, t_obj *obj, int i)
{
	t_ivec3 tex_index;
	t_ivec3 uv_index;
	int j = 0;

	uv_index.x = 0;
	uv_index.y = 0;
	uv_index.z = 0;
	while (str[j] != ' ')
		j++;
	j++;
	for (int x = 0; x < 3; x++)
	{
		if (x == 0)
			tex_index.x = atoi(&str[j]) - 1;
		else if (x == 1)
			tex_index.y = atoi(&str[j]) - 1;
		else if (x == 2)
			tex_index.z = atoi(&str[j]) - 1;
		while (str[j] && ft_isdigit(str[j]))
			j++;
		if (str[j] == '/')
		{
			j++;
			if (x == 0)
				uv_index.x = atoi(&str[j]) - 1;
			else if (x == 1)
				uv_index.y = atoi(&str[j]) - 1;
			else if (x == 2)
				uv_index.z = atoi(&str[j]) - 1;
			while (str[j] && str[j] != ' ')
				j++;
		}
		j++;
	}

	if (uvs)
	{
		obj->tris[i].verts[0].txtr[0] = uvs[uv_index.x].x;
		obj->tris[i].verts[0].txtr[1] = uvs[uv_index.x].y;
		obj->tris[i].verts[1].txtr[0] = uvs[uv_index.y].x;
		obj->tris[i].verts[1].txtr[1] = uvs[uv_index.y].y;
		obj->tris[i].verts[2].txtr[0] = uvs[uv_index.y].x;
		obj->tris[i].verts[2].txtr[1] = uvs[uv_index.y].y;
	}
	else
	{//idk if this block needed
		obj->tris[i].verts[0].txtr[0] = 0;
		obj->tris[i].verts[0].txtr[1] = 0;
		obj->tris[i].verts[1].txtr[0] = 0;
		obj->tris[i].verts[1].txtr[1] = 0;
		obj->tris[i].verts[2].txtr[0] = 0;
		obj->tris[i].verts[2].txtr[1] = 0;
	}
	obj->tris[i].verts[0].pos[0] = verts[tex_index.x].x;
	obj->tris[i].verts[0].pos[1] = -verts[tex_index.x].y;
	obj->tris[i].verts[0].pos[2] = -verts[tex_index.x].z;

	obj->tris[i].verts[1].pos[0] = verts[tex_index.y].x;
	obj->tris[i].verts[1].pos[1] = -verts[tex_index.y].y;
	obj->tris[i].verts[1].pos[2] = -verts[tex_index.y].z;

	obj->tris[i].verts[2].pos[0] = verts[tex_index.z].x;
	obj->tris[i].verts[2].pos[1] = -verts[tex_index.z].y;
	obj->tris[i].verts[2].pos[2] = -verts[tex_index.z].z;

	vec_sub(obj->tris[i].v0v2, obj->tris[i].verts[1].pos, obj->tris[i].verts[0].pos);
	vec_sub(obj->tris[i].v0v1, obj->tris[i].verts[2].pos, obj->tris[i].verts[0].pos);
}

void	set_mirror_dir(t_tri *a, int not_shared[2])
{
	float tmp[3];
	printf("dir = %d\n", not_shared[0]);

	/*if (not_shared[0] == 2)
	{
		vec_copy(tmp, a->verts[0].pos);
		vec_copy(a->verts[0].pos, a->verts[1].pos);
		vec_copy(a->verts[1].pos, a->verts[2].pos);
		vec_copy(a->verts[2].pos, tmp);
	}*/
	//else if (not_shared[0] == 1)
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

void	load_obj(char *filename, t_obj *obj)
{
	char **file;
	int i = 0;
	int tri_amount = 0;

	file = file2d(filename);
	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
			tri_amount++;
		i++;
	}
	printf("faces before optimized = %d\n", tri_amount);
	if (!(obj->tris = (t_tri *)malloc(sizeof(t_tri) * tri_amount)))
		ft_error("memory allocation failed\n");
	if (!(obj->distance_culling_mask = (int*)malloc(sizeof(int) * tri_amount)))
		ft_error("memory allocation failed\n");
	ft_memset(obj->distance_culling_mask, 0, tri_amount * sizeof(int));
	if (!(obj->backface_culling_mask = (int*)malloc(sizeof(int) * tri_amount)))
		ft_error("memory allocation failed\n");
	ft_memset(obj->backface_culling_mask, 1, tri_amount * sizeof(int));
	obj->tri_amount = tri_amount;
	t_vec3 *verts = load_verts(file);
	t_vec2 *uvs = load_uvs(file);
	i = 0;
	int j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
		{
			set_tri(file[i], verts, uvs, obj, j);
			obj->tris[j].isquad = 0;
			j++;
		}
		free(file[i]);
		i++;
	}
	free(file);
	free(verts);
	free(uvs);
	for (int i = 0; i < tri_amount; i++)
	{
		float y = obj->tris[i].verts[0].pos[1];
		if (obj->tris[i].verts[1].pos[1] == y &&
			obj->tris[i].verts[2].pos[1] == y)
			obj->distance_culling_mask[i] = 1;
	}
	find_quads(obj);
	printf("faces after optimized = %d\n", obj->tri_amount);
}
