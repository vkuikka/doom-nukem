/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/11 00:31:36 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static char	**file2d(char *filename)
{
	char	**file;
	char	*line;
	int		fd;
	int		i;

	i = 0;
	fd = open(filename, O_RDONLY);
	while (get_next_line(fd, &line))
	{
		free(line);
		i++;
	}
	free(line);
	close(fd);
	file = (char **)malloc(sizeof(char *) * (i + 1));
	fd = open(filename, O_RDONLY);
	if (fd < 2)
	{
		printf("%s\n", filename);
		ft_error("^: no such obj\n");
	}
	i = 0;
	while (get_next_line(fd, &line))
	{
		file[i] = line;
		i++;
	}
	free(line);
	file[i] = NULL;
	close(fd);
	return (file);
}

static void	set_uv_vert(char **file, int i, t_vec2 *vert)
{
	int	j;

	j = 0;
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
	int		i;
	int		uv_amount;
	t_vec2	*res;
	int		j;

	i = 0;
	uv_amount = 0;
	res = NULL;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "vt ", 3))
			uv_amount++;
		i++;
	}
	res = (t_vec2 *)malloc(sizeof(t_vec2) * uv_amount);
	if (!res)
		ft_error("memory allocation failed\n");
	i = 0;
	j = 0;
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
	int	j;

	j = 0;
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
	int		i;
	int		ver_amount;
	t_vec3	*res;
	int		j;

	i = 0;
	ver_amount = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "v ", 2))
			ver_amount++;
		i++;
	}
	res = (t_vec3 *)malloc(sizeof(t_vec3) * ver_amount);
	if (!res)
		ft_error("memory allocation failed\n");
	i = 0;
	j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "v ", 2))
		{
			set_vert(file, i, &res[j]);
			j++;
		}
		i++;
	}
	return (res);
}

static void	set_tri(char *str, t_vec3 *verts, t_vec2 *uvs, t_obj *obj, int i)
{
	t_ivec3	tex_index;
	t_ivec3	uv_index;
	int		j;
	int		x;

	j = 0;
	uv_index.x = 0;
	uv_index.y = 0;
	uv_index.z = 0;
	while (str[j] != ' ')
		j++;
	j++;
	x = -1;
	while (++x < 3)
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
	obj->tris[i].verts[0].pos.x = verts[tex_index.x].x;
	obj->tris[i].verts[0].pos.y = -verts[tex_index.x].y;
	obj->tris[i].verts[0].pos.z = -verts[tex_index.x].z;
	obj->tris[i].verts[1].pos.x = verts[tex_index.y].x;
	obj->tris[i].verts[1].pos.y = -verts[tex_index.y].y;
	obj->tris[i].verts[1].pos.z = -verts[tex_index.y].z;
	obj->tris[i].verts[2].pos.x = verts[tex_index.z].x;
	obj->tris[i].verts[2].pos.y = -verts[tex_index.z].y;
	obj->tris[i].verts[2].pos.z = -verts[tex_index.z].z;
}

void	load_obj(char *filename, t_obj *obj)
{
	char	**file;
	int		i;
	int		tri_amount;
	t_vec3	*verts;
	t_vec2	*uvs;
	int		j;

	i = 0;
	tri_amount = 0;
	file = file2d(filename);
	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
			tri_amount++;
		i++;
	}
	obj->tris = (t_tri *)malloc(sizeof(t_tri) * tri_amount);
	if (!obj->tris)
		ft_error("memory allocation failed\n");
	obj->tri_amount = tri_amount;
	verts = load_verts(file);
	uvs = load_uvs(file);
	i = 0;
	j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
		{
			ft_bzero(&obj->tris[j], sizeof(t_tri));
			set_tri(file[i], verts, uvs, obj, j);
			obj->tris[j].opacity = 0;
			obj->tris[j].reflectivity = 0;
			j++;
		}
		free(file[i]);
		i++;
	}
	free(file);
	free(verts);
	free(uvs);
	find_quads(obj);
	i = 0;
	while (i < obj->tri_amount)
	{
		vec_sub(&obj->tris[i].v0v2, obj->tris[i].verts[1].pos,
			obj->tris[i].verts[0].pos);
		vec_sub(&obj->tris[i].v0v1, obj->tris[i].verts[2].pos,
			obj->tris[i].verts[0].pos);
		vec_cross(&obj->tris[i].normal, obj->tris[i].v0v2, obj->tris[i].v0v1);
		vec_normalize(&obj->tris[i].normal);
		i++;
	}
}
