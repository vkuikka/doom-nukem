/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/02 14:19:35 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static char	**file2d(char *filename)
{
	char	**file;
	char	*line;
	int		fd;
	int		i;

	i = 0;
	fd = open(filename, O_RDONLY);
	if (fd < 2)
		return (NULL);
	while (get_next_line(fd, &line))
	{
		free(line);
		i++;
	}
	free(line);
	close(fd);
	file = (char **)malloc(sizeof(char *) * (i + 1));
	if (!file)
		return (NULL);
	fd = open(filename, O_RDONLY);
	if (fd < 2)
		return (NULL);
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
	int		j;
	int		uv_amount;
	t_vec2	*res;

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

void	read_indices(char *str, t_ivec3 *tex_index, t_ivec3 *uv_index)
{
	int		j;
	int		x;

	j = 0;
	while (str[j] != ' ')
		j++;
	j++;
	x = -1;
	while (++x < 3)
	{
		if (x == 0)
			tex_index->x = atoi(&str[j]) - 1;
		else if (x == 1)
			tex_index->y = atoi(&str[j]) - 1;
		else if (x == 2)
			tex_index->z = atoi(&str[j]) - 1;
		while (str[j] && ft_isdigit(str[j]))
			j++;
		if (str[j] == '/')
		{
			j++;
			if (x == 0)
				uv_index->x = atoi(&str[j]) - 1;
			else if (x == 1)
				uv_index->y = atoi(&str[j]) - 1;
			else if (x == 2)
				uv_index->z = atoi(&str[j]) - 1;
			while (str[j] && str[j] != ' ')
				j++;
		}
		j++;
	}
}

static int	set_tri(char *str, t_vec3 *verts, t_vec2 *uvs, t_tri *tri)
{
	t_ivec3	tex_index;
	t_ivec3	uv_index;

	tex_index = (t_ivec3){0, 0, 0};
	uv_index = (t_ivec3){0, 0, 0};
	read_indices(str, &tex_index, &uv_index);
	if (uvs)
	{
		tri->verts[0].txtr.x = uvs[uv_index.x].x;
		tri->verts[0].txtr.y = uvs[uv_index.x].y;
		tri->verts[1].txtr.x = uvs[uv_index.y].x;
		tri->verts[1].txtr.y = uvs[uv_index.y].y;
		tri->verts[2].txtr.x = uvs[uv_index.z].x;
		tri->verts[2].txtr.y = uvs[uv_index.z].y;
	}
	tri->verts[0].pos.x = verts[tex_index.x].x;
	tri->verts[0].pos.y = -verts[tex_index.x].y;
	tri->verts[0].pos.z = -verts[tex_index.x].z;
	tri->verts[1].pos.x = verts[tex_index.y].x;
	tri->verts[1].pos.y = -verts[tex_index.y].y;
	tri->verts[1].pos.z = -verts[tex_index.y].z;
	tri->verts[2].pos.x = verts[tex_index.z].x;
	tri->verts[2].pos.y = -verts[tex_index.z].y;
	tri->verts[2].pos.z = -verts[tex_index.z].z;
	return (TRUE);
}

static int	obj_set_all_tris(char **file, t_obj *obj)
{
	int		i;
	int		j;
	t_vec3	*verts;
	t_vec2	*uvs;

	verts = load_verts(file);
	if (!verts)
		return (FALSE);
	uvs = load_uvs(file);
	i = 0;
	j = 0;
	while (file[i])
	{
		if (file[i][0] == 'f' && file[i][1] == ' ')
		{
			if (!set_tri(file[i], verts, uvs, &obj->tris[j]))
				return (FALSE);
			j++;
		}
		i++;
	}
	free(verts);
	free(uvs);
	return (TRUE);
}

static int	obj_get_face_amount(char **file)
{
	int	i;
	int	res;

	i = 0;
	res = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
			res++;
		i++;
	}
	return (res);
}

static int	load_obj_internal(char **file, t_obj *obj)
{
	int		i;

	i = 0;
	obj->tri_amount = obj_get_face_amount(file);
	obj->tris = (t_tri *)malloc(sizeof(t_tri) * obj->tri_amount);
	if (!obj->tris)
		ft_error("memory allocation failed\n");
	ft_bzero(obj->tris, sizeof(t_tri) * obj->tri_amount);
	if (!obj_set_all_tris(file, obj))
		return (FALSE);
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
	return (TRUE);
}

int	load_obj(char *filename, t_obj *obj)
{
	char	**file;
	int		i;

	file = file2d(filename);
	if (!file)
		return (FALSE);
	if (!load_obj_internal(file, obj))
		return (FALSE);
	i = 0;
	while (file[i])
	{
		free(file[i]);
		i++;
	}
	free(file);
	return (TRUE);
}

void	load_obj_from_memory(unsigned char *data,
					unsigned int size, t_obj *obj)
{
	unsigned int	i;
	unsigned int	k;
	unsigned int	len;
	char			**file;

	i = 0;
	k = 0;
	while (i < size)
	{
		if ((char)data[i] == '\n')
			k++;
		i++;
	}
	file = (char **)malloc(sizeof(char *) * (k + 1));
	if (!file)
		ft_error("memory allocation failed");
	file[k] = NULL;
	i = 0;
	k = 0;
	len = 0;
	while (i < size)
	{
		if ((char)data[i] == '\n')
		{
			if (len)
				len--;
			file[k] = (char *)malloc(sizeof(char) * (len + 1));
			if (!file[k])
				ft_error("memory allocation failed");
			ft_strncpy(file[k], (char *)&data[i - len], len);
			file[k][len] = '\0';
			len = 0;
			k++;
		}
		len++;
		i++;
	}
	load_obj_internal(file, obj);
	i = 0;
	while (file[i])
	{
		free(file[i]);
		i++;
	}
	free(file);
}
