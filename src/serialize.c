/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serialize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/06 14:13:02 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/18 20:50:43 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	reserve_space(t_buffer *buf, size_t bytes)
{
	if ((buf->next + bytes) > buf->size)
	{
		if (!(buf->data = realloc(buf->data, buf->size * 2)))
			ft_error("memory allocation failed\n");
		buf->size *= 2;
	}
}

float	ntoh_float(float value)
{
	int temp;

	temp = ntohl(*(unsigned int*)&value);
	return *(float*)&temp;
}

void	deserialize_float(float *x, t_buffer *buf)
{
	if (buf->next + sizeof(float) > buf->size)
		ft_error("doom nukem file read fail");
	memcpy(x, ((char *)buf->data) + buf->next, sizeof(float));
	*x = ntoh_float(*x);
	buf->next += sizeof(float);
}

float	hton_float(float value)
{
	int temp;

	temp = htonl(*(unsigned int*)&value);
	return *(float*)&temp;
}

void	serialize_float(float x, t_buffer *buf)
{
	x = hton_float(x);
	reserve_space(buf, sizeof(float));
	memcpy(((char *)buf->data) + buf->next, &x, sizeof(float));
	buf->next += sizeof(float);
}

void	deserialize_int(int *x, t_buffer *buf)
{
	if (buf->next + sizeof(int) > buf->size)
		ft_error("doom nukem file read fail");
	memcpy(x, ((char *)buf->data) + buf->next, sizeof(int));
	*x = ntohl(*x);
	buf->next += sizeof(int);
}

void	serialize_int(int x, t_buffer *buf)
{
	x = htonl(x);
	reserve_space(buf, sizeof(int));
	memcpy(((char *)buf->data) + buf->next, &x, sizeof(int));
	buf->next += sizeof(int);
}

void	deserialize_vec2(t_vec2 *vec, t_buffer *buf)
{
	deserialize_float(&vec->x, buf);
	deserialize_float(&vec->y, buf);
}

void	serialize_vec2(t_vec2 vec, t_buffer *buf)
{
	serialize_float(vec.x, buf);
	serialize_float(vec.y, buf);
}

void	deserialize_vec3(t_vec3 *vec, t_buffer *buf)
{
	deserialize_float(&vec->x, buf);
	deserialize_float(&vec->y, buf);
	deserialize_float(&vec->z, buf);
}

void	serialize_vec3(t_vec3 vec, t_buffer *buf)
{
	serialize_float(vec.x, buf);
	serialize_float(vec.y, buf);
	serialize_float(vec.z, buf);
}

void	deserialize_settings(t_level *level, t_buffer *buf)
{
	deserialize_int(&level->ui.fog, buf);
	deserialize_int((int*)&level->ui.fog_color, buf);
	deserialize_int(&level->ui.backface_culling, buf);
	deserialize_int(&level->ui.distance_culling, buf);
	deserialize_float(&level->ui.render_distance, buf);
	deserialize_float(&level->ui.sun_contrast, buf);
	deserialize_float(&level->ui.direct_shadow_contrast, buf);
	deserialize_vec3(&level->ui.sun_dir, buf);
}

void	serialize_settings(t_level *level, t_buffer *buf)
{
	serialize_int(level->ui.fog, buf);
	serialize_int(level->ui.fog_color, buf);
	serialize_int(level->ui.backface_culling, buf);
	serialize_int(level->ui.distance_culling, buf);
	serialize_float(level->ui.render_distance, buf);
	serialize_float(level->ui.sun_contrast, buf);
	serialize_float(level->ui.direct_shadow_contrast, buf);
	serialize_vec3(level->ui.sun_dir, buf);
}

void	deserialize_vert(t_vert *vert, t_buffer *buf)
{
	deserialize_vec3(&vert->pos, buf);
	deserialize_vec2(&vert->txtr, buf);
}

void	serialize_vert(t_vert *vert, t_buffer *buf)
{
	serialize_vec3(vert->pos, buf);
	serialize_vec2(vert->txtr, buf);
}

void	deserialize_enemy(t_enemy *enemy, t_buffer *buf)
{
	deserialize_vec3(&enemy->dir, buf);
	for (int i = 0; i < 3; i++)
		deserialize_vec2(&enemy->projectile_uv[i], buf);
	deserialize_float(&enemy->move_speed, buf);
	deserialize_float(&enemy->dist_limit, buf);
	deserialize_float(&enemy->initial_health, buf);
	deserialize_float(&enemy->remaining_health, buf);
	deserialize_float(&enemy->attack_range, buf);
	deserialize_float(&enemy->attack_frequency, buf);
	deserialize_float(&enemy->projectile_speed, buf);
	deserialize_float(&enemy->attack_damage, buf);
	deserialize_float(&enemy->current_attack_delay, buf);
}

void	serialize_enemy(t_enemy *enemy, t_buffer *buf)
{
	serialize_vec3(enemy->dir, buf);
	for (int i = 0; i < 3; i++)
		serialize_vec2(enemy->projectile_uv[i], buf);
	serialize_float(enemy->move_speed, buf);
	serialize_float(enemy->dist_limit, buf);
	serialize_float(enemy->initial_health, buf);
	serialize_float(enemy->remaining_health, buf);
	serialize_float(enemy->attack_range, buf);
	serialize_float(enemy->attack_frequency, buf);
	serialize_float(enemy->projectile_speed, buf);
	serialize_float(enemy->attack_damage, buf);
	serialize_float(enemy->current_attack_delay, buf);
}

void	deserialize_projectile(t_projectile *projectile, t_buffer *buf)
{
	deserialize_vec3(&projectile->dir, buf);
	deserialize_float(&projectile->speed, buf);
	deserialize_float(&projectile->dist, buf);
	deserialize_float(&projectile->damage, buf);
}

void	serialize_projectile(t_projectile *projectile, t_buffer *buf)
{
	serialize_vec3(projectile->dir, buf);
	serialize_float(projectile->speed, buf);
	serialize_float(projectile->dist, buf);
	serialize_float(projectile->damage, buf);
}

void	deserialize_tri(t_tri *tri, t_buffer *buf)
{
	for (int i = 0; i < 4; i++)
		deserialize_vert(&tri->verts[i], buf);
	deserialize_vec3(&tri->v0v1, buf);
	deserialize_vec3(&tri->v0v2, buf);
	deserialize_vec3(&tri->normal, buf);
	deserialize_int(&tri->isquad, buf);
	deserialize_int(&tri->isgrid, buf);
	deserialize_int(&tri->isenemy, buf);
	deserialize_int(&tri->isprojectile, buf);
	deserialize_float(&tri->opacity, buf);
	deserialize_float(&tri->reflectivity, buf);
	deserialize_int(&tri->shader, buf);
	if (tri->isenemy)
	{
		if (!(tri->enemy = (t_enemy*)malloc(sizeof(t_enemy))))
			ft_error("failed to allocate memory for file");
		deserialize_enemy(tri->enemy, buf);
	}
	if (tri->isprojectile)
	{
		if (!(tri->projectile = (t_projectile*)malloc(sizeof(t_projectile))))
			ft_error("failed to allocate memory for file");
		deserialize_projectile(tri->projectile, buf);
	}
}

void	serialize_tri(t_tri *tri, t_buffer *buf)
{
	for (int i = 0; i < 4; i++)
		serialize_vert(&tri->verts[i], buf);
	serialize_vec3(tri->v0v1, buf);
	serialize_vec3(tri->v0v2, buf);
	serialize_vec3(tri->normal, buf);
	serialize_int(tri->isquad, buf);
	serialize_int(tri->isgrid, buf);
	serialize_int(tri->isenemy, buf);
	serialize_int(tri->isprojectile, buf);
	serialize_float(tri->opacity, buf);
	serialize_float(tri->reflectivity, buf);
	serialize_int(tri->shader, buf);
	if (tri->isenemy)
		serialize_enemy(tri->enemy, buf);
	if (tri->isprojectile)
		serialize_projectile(tri->projectile, buf);
}

void	deserialize_obj(t_obj *obj, t_buffer *buf)
{
	deserialize_int(&obj->tri_amount, buf);
	if (!(obj->tris = (t_tri*)malloc(sizeof(t_tri) * obj->tri_amount)))
		ft_error("failed to allocate memory for file");
	ft_bzero(obj->tris, sizeof(t_tri) * obj->tri_amount);
	for (int i = 0; i < obj->tri_amount; i++)
		deserialize_tri(&obj->tris[i], buf);
}

void	serialize_obj(t_obj *obj, t_buffer *buf)
{
	serialize_int(obj->tri_amount, buf);
	for (int i = 0; i < obj->tri_amount; i++)
		serialize_tri(&obj->tris[i], buf);
}

void	deserialize_bmp(t_bmp *bmp, t_buffer *buf)
{
	deserialize_int(&bmp->width, buf);
	deserialize_int(&bmp->height, buf);
	if (!(bmp->image = (int*)malloc(sizeof(int) * bmp->width * bmp->height)))
		ft_error("failed to allocate memory for file");
	for (int y = 0; y < bmp->height; y++)
		for (int x = 0; x < bmp->width; x++)
			deserialize_int(&bmp->image[bmp->width * y + x], buf);
}

void	serialize_bmp(t_bmp *bmp, t_buffer *buf)
{
	serialize_int(bmp->width, buf);
	serialize_int(bmp->height, buf);
	for (int y = 0; y < bmp->height; y++)
		for (int x = 0; x < bmp->width; x++)
			serialize_int(bmp->image[bmp->width * y + x], buf);
}

char	*deserialize_string(int len, t_buffer *buf)
{
	char	*str;
	int		get;
	int		i;

	str = (char*)malloc(sizeof(char) * len + 1);
	i = 0;
	while (i < len)
	{
		deserialize_int(&get, buf);
		str[i] = (char)get;
		i++;
	}
	str[i] = '\0';
	return (str);
}

void	serialize_string(char *str, t_buffer *buf)
{
	int i;

	i = 0;
	while (str[i])
	{
		serialize_int(str[i], buf);
		i++;
	}
}

void	deserialize_door(t_door *door, t_buffer *buf)
{
	deserialize_int(&door->indice_amount, buf);
	if (!(door->indices = (int*)malloc(sizeof(int) * door->indice_amount)))
		ft_error("memory allocation failed\n");
	if (!(door->isquad = (int*)malloc(sizeof(int) * door->indice_amount)))
		ft_error("memory allocation failed\n");
	if (!(door->pos1 = (t_vec3**)malloc(sizeof(t_vec3*) * door->indice_amount)))
		ft_error("memory allocation failed\n");
	if (!(door->pos2 = (t_vec3**)malloc(sizeof(t_vec3*) * door->indice_amount)))
		ft_error("memory allocation failed\n");
	for (int i = 0; i < door->indice_amount; i++)
	{
		deserialize_int(&door->indices[i], buf);
		deserialize_int(&door->isquad[i], buf);
		if (!(door->pos1[i] = (t_vec3*)malloc(sizeof(t_vec3) * 4)))
			ft_error("memory allocation failed\n");
		if (!(door->pos2[i] = (t_vec3*)malloc(sizeof(t_vec3) * 4)))
			ft_error("memory allocation failed\n");
		for (int k = 0; k < 4; k++)
		{
			deserialize_vec3(&door->pos1[i][k], buf);
			deserialize_vec3(&door->pos2[i][k], buf);
		}
	}
	deserialize_float(&door->transition_time, buf);
}

void	serialize_door(t_door *door, t_buffer *buf)
{
	serialize_int(door->indice_amount, buf);
	for (int i = 0; i < door->indice_amount; i++)
	{
		serialize_int(door->indices[i], buf);
		serialize_int(door->isquad[i], buf);
		for (int k = 0; k < 4; k++)
		{
			serialize_vec3(door->pos1[i][k], buf);
			serialize_vec3(door->pos2[i][k], buf);
		}
	}
	serialize_float(door->transition_time, buf);
}

void	deserialize_doors(t_level *level, t_buffer *buf)
{
	for (int i = 0; i < level->doors.door_amount; i++)
	{
		free(level->doors.door[i].indices);
		free(level->doors.door[i].isquad);
		for (int k = 0; k < level->doors.door[i].indice_amount; k++)
		{
			free(level->doors.door[i].pos1[k]);
			free(level->doors.door[i].pos2[k]);
		}
		free(level->doors.door[i].pos1);
		free(level->doors.door[i].pos2);
	}
	free(level->doors.door);
	deserialize_int(&level->doors.door_amount, buf);
	if (!(level->doors.door = (t_door*)malloc(sizeof(t_door) * level->doors.door_amount)))
		ft_error("memory allocation failed\n");
	for (int i = 0; i < level->doors.door_amount; i++)
		deserialize_door(&level->doors.door[i], buf);
}

void	serialize_doors(t_level *level, t_buffer *buf)
{
	serialize_int(level->doors.door_amount, buf);
	for (int i = 0; i < level->doors.door_amount; i++)
		serialize_door(&level->doors.door[i], buf);
}

void	deserialize_level(t_level *level, t_buffer *buf)
{
	char	*str;

	str = deserialize_string(ft_strlen("doom-nukem"), buf);
	if (ft_strcmp(str, "doom-nukem"))
	{
		nonfatal_error(level, "not valid doom-nukem map");
		free(str);
		return ;
	}
	free(str);
	deserialize_settings(level, buf);

	free(level->texture.image);
	deserialize_bmp(&level->texture, buf);

	free(level->normal_map.image);
	deserialize_bmp(&level->normal_map, buf);

	free(level->sky.img.image);
	deserialize_bmp(&level->sky.img, buf);

	free_culling(level);
	free(level->all.tris);
	free(level->visible.tris);
	deserialize_obj(&level->all, buf);
	deserialize_doors(level, buf);
	if (!(level->visible.tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	init_screen_space_partition(level);
	init_culling(level);
}

void	serialize_level(t_level *level, t_buffer *buf)
{
	serialize_string("doom-nukem", buf);
	serialize_settings(level, buf);
	serialize_bmp(&level->texture, buf);
	serialize_bmp(&level->normal_map, buf);
	serialize_bmp(&level->sky.img, buf);
	serialize_obj(&level->all, buf);
	serialize_doors(level, buf);
}

#ifdef _WIN32
void	save_file(t_level *level, t_buffer *buf)
{
	HANDLE	hFile;
	char	*filename1;
	char	*filename2;
	DWORD	bytesWritten;

	filename1 = ft_strjoin(level->ui.state.directory, "\\");
	filename2 = ft_strjoin(filename1, level->ui.state.save_filename);
	free(filename1);
	filename1 = ft_strjoin(filename2, ".doom-nukem");
	free(filename2);
	hFile = CreateFile(filename1, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		nonfatal_error(level, "failed to create file");
	else
		WriteFile(hFile, buf->data, buf->next, &bytesWritten, NULL);
	free(filename1);
	CloseHandle(hFile);
}

void	open_file(char *filename, t_buffer *buf)
{
	HANDLE	hFile;
	DWORD	nRead;

	hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		ft_error("failed to read file");
	size_t file_size = (size_t)GetFileSize(hFile, NULL);
	if (!(buf->data = malloc(file_size + 1)))
		ft_error("failed to allocate memory for file");
	if (!ReadFile(hFile, buf->data, file_size, &nRead, NULL))
		ft_error("failed to read file");
	buf->size = nRead;
	CloseHandle(hFile);
}

#elif __APPLE__
void	save_file(t_level *level, t_buffer *buf)
{
	int		fd;
	char	*filename1;
	char	*filename2;

	filename1 = ft_strjoin(level->ui.state.directory, "/");
	filename2 = ft_strjoin(filename1, level->ui.state.save_filename);
	free(filename1);
	filename1 = ft_strjoin(filename2, ".doom-nukem");
	free(filename2);
	fd = open(filename1, O_RDWR | O_CREAT,
			S_IRGRP | S_IWGRP | S_IXGRP |
			S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd < 3)
		nonfatal_error(level, "failed to create file");
	else if (write(fd, buf->data, buf->next) != buf->next)
		nonfatal_error(level, "failed to write file");
	free(filename1);
	close(fd);
}

void	open_file(char *filename, t_buffer *buf)
{
	struct stat	fstat;
	int			fd;

	fd = open(filename, O_RDONLY);
	if (fd < 3)
		ft_error("failed to read file");
	stat(filename, &fstat);
	buf->size = fstat.st_size;
	if (!(buf->data = malloc(buf->size + 1)))
		ft_error("failed to allocate memory for file");
	if (read(fd, buf->data, buf->size) != buf->size)
		ft_error("failed to read file");
	close(fd);
}
#endif

void	save_level(t_level *level)
{
	t_buffer	buf;

	if (!level->ui.state.save_filename[0] || level->ui.state.save_filename[0] == '.')
	{
		nonfatal_error(level, "Invalid filename");
		return ;
	}
	if (!(buf.data = malloc(SERIALIZE_INITIAL_BUFFER_SIZE)))
		ft_error("memory allocation failed\n");
	buf.size = SERIALIZE_INITIAL_BUFFER_SIZE;
	buf.next = 0;
	serialize_level(level, &buf);
	save_file(level, &buf);
}

void	open_level(t_level *level, char *filename)
{
	t_buffer	buf;

	buf.data = 0;
	buf.next = 0;
	buf.size = 0;

	//sync physics thread with mutex
	open_file(filename, &buf);
	//free level things
	deserialize_level(level, &buf);
	free(buf.data);
}