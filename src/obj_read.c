/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2022/01/19 17:58:16 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

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
	int		uv_amount;
	t_vec2	*res;
	int		j;
	int		i;

	i = 0;
	uv_amount = 0;
	res = NULL;
	while (file[i])
		if (!ft_strncmp(file[i++], "vt ", 3))
			uv_amount++;
	if (!uv_amount)
		return (NULL);
	res = (t_vec2 *)malloc(sizeof(t_vec2) * uv_amount);
	if (!res)
		ft_error("memory allocation failed\n");
	i = 0;
	j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "vt ", 3))
			set_uv_vert(file, i, &res[j++]);
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
	int		ver_amount;
	t_vec3	*res;
	int		j;
	int		i;

	i = 0;
	ver_amount = 0;
	while (file[i])
		if (!ft_strncmp(file[i++], "v ", 2))
			ver_amount++;
	if (!ver_amount)
		return (NULL);
	res = (t_vec3 *)malloc(sizeof(t_vec3) * ver_amount);
	if (!res)
		ft_error("memory allocation failed\n");
	i = 0;
	j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "v ", 2))
			set_vert(file, i, &res[j++]);
		i++;
	}
	return (res);
}

static void	read_uv_indices(char *str, t_ivec3 *uv_index, int *j, int x)
{
	(*j)++;
	if (x == 0)
		uv_index->x = ft_atoi(&str[*j]) - 1;
	else if (x == 1)
		uv_index->y = ft_atoi(&str[*j]) - 1;
	else if (x == 2)
		uv_index->z = ft_atoi(&str[*j]) - 1;
	while (str[*j] && str[*j] != ' ')
		(*j)++;
}

int	read_indices(char *str, t_ivec3 *tex_index, t_ivec3 *uv_index)
{
	int		j;
	int		x;

	*tex_index = (t_ivec3){0, 0, 0};
	*uv_index = (t_ivec3){0, 0, 0};
	j = 0;
	while (str[j] != ' ')
		j++;
	j++;
	x = -1;
	while (++x < 3)
	{
		if (x == 0)
			tex_index->x = ft_atoi(&str[j]) - 1;
		else if (x == 1)
			tex_index->y = ft_atoi(&str[j]) - 1;
		else if (x == 2)
			tex_index->z = ft_atoi(&str[j]) - 1;
		while (str[j] && ft_isdigit(str[j]))
			j++;
		if (str[j] == '/')
			read_uv_indices(str, uv_index, &j, x);
		j++;
	}
	return (str[j - 1] == '\0');
}

static int	set_tri(char *str, t_vec3 *verts, t_vec2 *uvs, t_tri *tri)
{
	t_ivec3	tex_index;
	t_ivec3	uv_index;

	if (!read_indices(str, &tex_index, &uv_index))
		return (FALSE);
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

static int	obj_set_all_tris_res(t_vec3 *verts, t_vec2 *uvs, int res)
{
	free(verts);
	if (uvs)
		free(uvs);
	if (res == FALSE)
		nonfatal_error("obj not triangulated");
	return (res);
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
	if (!uvs)
		nonfatal_error("obj has no uv");
	i = 0;
	j = 0;
	while (file[i])
	{
		if (file[i][0] == 'f' && file[i][1] == ' '
			&& !set_tri(file[i], verts, uvs, &obj->tris[j++]))
			return (obj_set_all_tris_res(verts, uvs, FALSE));
		i++;
	}
	return (obj_set_all_tris_res(verts, uvs, TRUE));
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

void	tri_optimize(t_tri *tri)
{
	vec_sub(&tri->v0v2, tri->verts[1].pos, tri->verts[0].pos);
	vec_sub(&tri->v0v1, tri->verts[2].pos, tri->verts[0].pos);
	vec_cross(&tri->normal, tri->v0v2, tri->v0v1);
	vec_normalize(&tri->normal);
}

static int	load_obj_internal(char **file, t_obj *obj)
{
	int		i;

	i = 0;
	obj->tri_amount = obj_get_face_amount(file);
	obj->tris = (t_tri *)malloc(sizeof(t_tri) * obj->tri_amount);
	if (!obj->tris || !obj->tri_amount)
		return (FALSE);
	ft_bzero(obj->tris, sizeof(t_tri) * obj->tri_amount);
	if (!obj_set_all_tris(file, obj))
		return (FALSE);
	find_quads(obj);
	i = 0;
	while (i < obj->tri_amount)
	{
		obj->tris[i].index = i;
		tri_optimize(&obj->tris[i]);
		i++;
	}
	return (TRUE);
}

int	load_obj(char *filename, t_obj *obj)
{
	char	**file;
	int		res;

	file = file2d(filename);
	if (!file)
		return (FALSE);
	res = TRUE;
	if (!load_obj_internal(file, obj))
		res = FALSE;
	free_file2d(file);
	return (res);
}

void	load_obj_from_memory(unsigned char *data,
					unsigned int size, t_obj *obj)
{
	char	**file;

	file = file2d_from_memory(data, size);
	if (!load_obj_internal(file, obj))
		ft_error("obj memory read fail");
	free_file2d(file);
}
