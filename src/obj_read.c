/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/29 03:45:11 by vkuikka          ###   ########.fr       */
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
		obj->tris[i].verts[0].txtr.x = uvs[uv_index.x].x;
		obj->tris[i].verts[0].txtr.y = uvs[uv_index.x].y;
		obj->tris[i].verts[1].txtr.x = uvs[uv_index.y].x;
		obj->tris[i].verts[1].txtr.y = uvs[uv_index.y].y;
		obj->tris[i].verts[2].txtr.x = uvs[uv_index.z].x;
		obj->tris[i].verts[2].txtr.y = uvs[uv_index.z].y;
	}
	else
	{//idk if this block needed
		obj->tris[i].verts[0].txtr.x = 0;
		obj->tris[i].verts[0].txtr.y = 0;
		obj->tris[i].verts[1].txtr.x = 0;
		obj->tris[i].verts[1].txtr.y = 0;
		obj->tris[i].verts[2].txtr.x = 0;
		obj->tris[i].verts[2].txtr.y = 0;
	}
	obj->tris[i].verts[0].pos.x = verts[tex_index.x].x;
	obj->tris[i].verts[0].pos.y = -verts[tex_index.x].y;
	obj->tris[i].verts[0].pos.z = -verts[tex_index.x].z;

	obj->tris[i].verts[1].pos.x = verts[tex_index.y].x;
	obj->tris[i].verts[1].pos.y = -verts[tex_index.y].y;
	obj->tris[i].verts[1].pos.z = -verts[tex_index.y].z;

	obj->tris[i].verts[2].pos.x = verts[tex_index.z].x;
	obj->tris[i].verts[2].pos.y = -verts[tex_index.z].y;
	obj->tris[i].verts[2].pos.z = -verts[tex_index.z].z;

	vec_sub(&obj->tris[i].v0v2, obj->tris[i].verts[1].pos, obj->tris[i].verts[0].pos);
	vec_sub(&obj->tris[i].v0v1, obj->tris[i].verts[2].pos, obj->tris[i].verts[0].pos);
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
	printf("faces = %d\n", tri_amount);
	if (!(obj->tris = (t_tri *)malloc(sizeof(t_tri) * tri_amount)))
		ft_error("memory allocation failed\n");
	if (!(obj->distance_culling_mask = (int*)malloc(sizeof(int) * tri_amount)))
		ft_error("memory allocation failed\n");
	ft_memset(obj->distance_culling_mask, 0, tri_amount * sizeof(int));
	if (!(obj->backface_culling_mask = (int*)malloc(sizeof(int) * tri_amount)))
		ft_error("memory allocation failed\n");
	ft_memset(obj->backface_culling_mask, 0, tri_amount * sizeof(int));
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
	/*for (int i = 0; i < tri_amount; i++)
	{
		float y = obj->tris[i].verts[0].pos[1];
		if (obj->tris[i].verts[1].pos[1] == y &&
			obj->tris[i].verts[2].pos[1] == y)
			obj->distance_culling_mask[i] = 1;
	}*/
	find_quads(obj);
	printf("faces = %d\n", obj->tri_amount);
}
