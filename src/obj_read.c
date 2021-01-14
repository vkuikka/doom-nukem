/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/12 01:20:06 by rpehkone         ###   ########.fr       */
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

static void	set_vert(char **file, int i, t_vec3 *vert)
{
	int j = 0;

	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->x = atof(&file[i][j]);
	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->y = atof(&file[i][j]);
	while (file[i][j] != ' ')
	{
		j++;
	}
	j++;
	vert->z = atof(&file[i][j]);
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

static void	set_tri(char *str, t_vec3 *verts, t_obj *obj, int i)
{
	t_ivec3 index;
	int j = 0;

	while (str[j] != ' ')
	{
		j++;
	}
	j++;
	index.x = atoi(&str[j]);
	while (str[j] != ' ')
	{
		j++;
	}
	j++;
	index.y = atoi(&str[j]);
	while (str[j] != ' ')
	{
		j++;
	}
	j++;
	index.z = atoi(&str[j]);

	obj->tris[i].verts[0].pos[0] = verts[index.x].x;
	obj->tris[i].verts[0].pos[1] = verts[index.x].y;
	obj->tris[i].verts[0].pos[2] = verts[index.x].z;

	obj->tris[i].verts[1].pos[0] = verts[index.y].x;
	obj->tris[i].verts[1].pos[1] = verts[index.y].y;
	obj->tris[i].verts[1].pos[2] = verts[index.y].z;

	obj->tris[i].verts[2].pos[0] = verts[index.z].x;
	obj->tris[i].verts[2].pos[1] = verts[index.z].y;
	obj->tris[i].verts[2].pos[2] = verts[index.z].z;

	vec_sub(obj->tris[i].v0v1, obj->tris[i].verts[1].pos, obj->tris[i].verts[0].pos);
	vec_sub(obj->tris[i].v0v2, obj->tris[i].verts[2].pos, obj->tris[i].verts[0].pos);
}

void	load_obj(char *filename, t_obj *obj)
{
	char **file;

	file = file2d(filename);
	int i = 0;
	int tri_amount = 0;

	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
			tri_amount++;
		i++;
	}
	printf("faces = %d\n", tri_amount);
	if (!(obj->tris = (t_tri *)malloc(sizeof(t_tri) * tri_amount)))
		ft_error("memory allocation failed\n");

	obj->tri_amount = tri_amount;

	t_vec3 *verts = load_verts(file);

	i = 0;
	int j = 0;
	while (file[i])
	{
		if (!ft_strncmp(file[i], "f ", 2))
		{
			set_tri(file[i], verts, obj, j);
			j++;
		}
		i++;
	}
}
