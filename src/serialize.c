/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serialize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/06 14:13:02 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/27 03:02:27 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	reserve_space(t_buffer *buf, size_t bytes)
{
	if ((buf->next + bytes) > buf->size)
	{
		buf->data = realloc(buf->data, buf->size * 2);
		if (!buf->data)
			ft_error("memory allocation failed\n");
		buf->size *= 2;
	}
}

// Gcc assumes that your program will never access
// variables though pointers of different type.
// This assumption is called strict-aliasing and allows
// the compiler to make some optimizations.
// Strict-aliasing rule says that a char* and
// void* can point at any type.
float	ntoh_float(float value)
{
	int		temp;
	void	*cast;

	cast = &value;
	temp = ntohl(*(unsigned int *)cast);
	cast = &temp;
	return (*(float *)cast);
}

void	deserialize_float(float *x, t_buffer *buf)
{
	if (buf->next + sizeof(float) > buf->size)
		ft_error("doom nukem file read fail");
	ft_memcpy(x, ((char *)buf->data) + buf->next, sizeof(float));
	*x = ntoh_float(*x);
	buf->next += sizeof(float);
}

float	hton_float(float value)
{
	int		temp;
	void	*cast;

	cast = &value;
	temp = htonl(*(unsigned int *)cast);
	cast = &temp;
	return (*(float *)cast);
}

void	serialize_float(float x, t_buffer *buf)
{
	x = hton_float(x);
	reserve_space(buf, sizeof(float));
	ft_memcpy(((char *)buf->data) + buf->next, &x, sizeof(float));
	buf->next += sizeof(float);
}

void	deserialize_int(int *x, t_buffer *buf)
{
	if (buf->next + sizeof(int) > buf->size)
		ft_error("doom nukem file read fail");
	ft_memcpy(x, ((char *)buf->data) + buf->next, sizeof(int));
	*x = ntohl(*x);
	buf->next += sizeof(int);
}

void	serialize_int(int x, t_buffer *buf)
{
	x = htonl(x);
	reserve_space(buf, sizeof(int));
	ft_memcpy(((char *)buf->data) + buf->next, &x, sizeof(int));
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

void	deserialize_color(t_color_hsl *color, t_buffer *buf)
{
	deserialize_float(&color->hue, buf);
	deserialize_float(&color->saturation, buf);
	deserialize_float(&color->lightness, buf);
	deserialize_int(&color->color, buf);
	deserialize_float(&color->r, buf);
	deserialize_float(&color->g, buf);
	deserialize_float(&color->b, buf);
}

void	serialize_color(t_color_hsl color, t_buffer *buf)
{
	serialize_float(color.hue, buf);
	serialize_float(color.saturation, buf);
	serialize_float(color.lightness, buf);
	serialize_int(color.color, buf);
	serialize_float(color.r, buf);
	serialize_float(color.g, buf);
	serialize_float(color.b, buf);
}

void	deserialize_settings(t_level *level, t_buffer *buf)
{
	deserialize_int(&level->ui.fog, buf);
	deserialize_int((int *)&level->ui.fog_color.color, buf);
	deserialize_int(&level->ui.backface_culling, buf);
	deserialize_int(&level->ui.occlusion_culling, buf);
	deserialize_int(&level->ui.distance_culling, buf);
	deserialize_float(&level->ui.render_distance, buf);
	deserialize_color(&level->ui.sun_color, buf);
	deserialize_vec3(&level->ui.sun_dir, buf);
	deserialize_vec3(&level->game_logic.win_pos, buf);
	deserialize_float(&level->game_logic.win_dist, buf);
}

void	serialize_settings(t_level *level, t_buffer *buf)
{
	serialize_int(level->ui.fog, buf);
	serialize_int(level->ui.fog_color.color, buf);
	serialize_int(level->ui.backface_culling, buf);
	serialize_int(level->ui.occlusion_culling, buf);
	serialize_int(level->ui.distance_culling, buf);
	serialize_float(level->ui.render_distance, buf);
	serialize_color(level->ui.sun_color, buf);
	serialize_vec3(level->ui.sun_dir, buf);
	serialize_vec3(level->game_logic.win_pos, buf);
	serialize_float(level->game_logic.win_dist, buf);
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
	int	i;

	i = 0;
	deserialize_vec3(&enemy->dir, buf);
	while (i < 3)
	{
		deserialize_vec2(&enemy->projectile_uv[i], buf);
		i++;
	}
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
	int	i;

	i = 0;
	serialize_vec3(enemy->dir, buf);
	while (i < 3)
	{
		serialize_vec2(enemy->projectile_uv[i], buf);
		i++;
	}
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

void	deserialize_player_pos(t_player_pos *pos, t_buffer *buf)
{
	deserialize_vec3(&pos->pos, buf);
	deserialize_float(&pos->look_side, buf);
	deserialize_float(&pos->look_up, buf);
}

void	serialize_player_pos(t_player_pos *pos, t_buffer *buf)
{
	serialize_vec3(pos->pos, buf);
	serialize_float(pos->look_side, buf);
	serialize_float(pos->look_up, buf);
}

void	deserialize_tri(t_tri *tri, t_buffer *buf)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		deserialize_vert(&tri->verts[i], buf);
		i++;
	}
	deserialize_vec3(&tri->v0v1, buf);
	deserialize_vec3(&tri->v0v2, buf);
	deserialize_vec3(&tri->normal, buf);
	deserialize_int(&tri->isquad, buf);
	deserialize_int(&tri->isgrid, buf);
	deserialize_int(&tri->isenemy, buf);
	deserialize_int(&tri->isprojectile, buf);
	deserialize_float(&tri->opacity, buf);
	deserialize_float(&tri->reflectivity, buf);
	deserialize_float(&tri->refractivity, buf);
	deserialize_int((int*)&tri->shader, buf);
	if (tri->isenemy)
	{
		tri->enemy = (t_enemy *)malloc(sizeof(t_enemy));
		if (!tri->enemy)
			ft_error("failed to allocate memory for file");
		deserialize_enemy(tri->enemy, buf);
	}
	if (tri->isprojectile)
	{
		tri->projectile = (t_projectile *)malloc(sizeof(t_projectile));
		if (!tri->projectile)
			ft_error("failed to allocate memory for file");
		deserialize_projectile(tri->projectile, buf);
	}
}

void	serialize_tri(t_tri *tri, t_buffer *buf)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		serialize_vert(&tri->verts[i], buf);
		i++;
	}
	serialize_vec3(tri->v0v1, buf);
	serialize_vec3(tri->v0v2, buf);
	serialize_vec3(tri->normal, buf);
	serialize_int(tri->isquad, buf);
	serialize_int(tri->isgrid, buf);
	serialize_int(tri->isenemy, buf);
	serialize_int(tri->isprojectile, buf);
	serialize_float(tri->opacity, buf);
	serialize_float(tri->reflectivity, buf);
	serialize_float(tri->refractivity, buf);
	serialize_int((int)tri->shader, buf);
	if (tri->isenemy)
		serialize_enemy(tri->enemy, buf);
	if (tri->isprojectile)
		serialize_projectile(tri->projectile, buf);
}

void	deserialize_obj(t_obj *obj, t_buffer *buf)
{
	int	i;

	i = 0;
	deserialize_int(&obj->tri_amount, buf);
	obj->tris = (t_tri *)malloc(sizeof(t_tri) * obj->tri_amount);
	if (!obj->tris)
		ft_error("failed to allocate memory for file");
	ft_bzero(obj->tris, sizeof(t_tri) * obj->tri_amount);
	while (i < obj->tri_amount)
	{
		deserialize_tri(&obj->tris[i], buf);
		obj->tris[i].index = i;
		i++;
	}
}

void	serialize_obj(t_obj *obj, t_buffer *buf)
{
	int	i;

	i = 0;
	serialize_int(obj->tri_amount, buf);
	while (i < obj->tri_amount)
	{
		serialize_tri(&obj->tris[i], buf);
		i++;
	}
}

void	deserialize_bmp(t_bmp *bmp, t_buffer *buf)
{
	int	x;
	int	y;

	deserialize_int(&bmp->width, buf);
	deserialize_int(&bmp->height, buf);
	bmp->image = (int *)malloc(sizeof(int) * bmp->width * bmp->height);
	if (!bmp->image)
		ft_error("failed to allocate memory for file");
	y = 0;
	while (y < bmp->height)
	{
		x = 0;
		while (x < bmp->width)
		{
			deserialize_int(&bmp->image[bmp->width * y + x], buf);
			x++;
		}
		y++;
	}
}

void	serialize_bmp(t_bmp *bmp, t_buffer *buf)
{
	int	x;
	int	y;

	serialize_int(bmp->width, buf);
	serialize_int(bmp->height, buf);
	y = 0;
	while (y < bmp->height)
	{
		x = 0;
		while (x < bmp->width)
		{
			serialize_int(bmp->image[bmp->width * y + x], buf);
			x++;
		}
		y++;
	}
}

char	*deserialize_string(int len, t_buffer *buf)
{
	char	*str;
	int		get;
	int		i;

	str = (char *)malloc(sizeof(char) * len + 1);
	if (!str)
		ft_error("serialize memory allocation failed");
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
	int	i;

	i = 0;
	while (str[i])
	{
		serialize_int(str[i], buf);
		i++;
	}
}

void	deserialize_door(t_door *door, t_buffer *buf)
{
	int	i;
	int	k;

	i = 0;
	deserialize_int(&door->indice_amount, buf);
	door->indices = (int *)malloc(sizeof(int) * door->indice_amount);
	if (!door->indices)
		ft_error("memory allocation failed\n");
	door->isquad = (int *)malloc(sizeof(int) * door->indice_amount);
	if (!door->isquad)
		ft_error("memory allocation failed\n");
	door->pos1 = (t_vec3 **)malloc(sizeof(t_vec3 *) * door->indice_amount);
	if (!door->pos1)
		ft_error("memory allocation failed\n");
	door->pos2 = (t_vec3 **)malloc(sizeof(t_vec3 *) * door->indice_amount);
	if (!door->pos2)
		ft_error("memory allocation failed\n");
	while (i < door->indice_amount)
	{
		deserialize_int(&door->indices[i], buf);
		deserialize_int(&door->isquad[i], buf);
		door->pos1[i] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
		if (!door->pos1[i])
			ft_error("memory allocation failed\n");
		door->pos2[i] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
		if (!door->pos2[i])
			ft_error("memory allocation failed\n");
		k = 0;
		while (k < 4)
		{
			deserialize_vec3(&door->pos1[i][k], buf);
			deserialize_vec3(&door->pos2[i][k], buf);
			k++;
		}
		i++;
	}
	deserialize_int(&door->is_activation_pos_active, buf);
	deserialize_vec3(&door->activation_pos, buf);
	deserialize_float(&door->transition_time, buf);
}

void	serialize_door(t_door *door, t_buffer *buf)
{
	int	i;
	int	k;

	i = 0;
	serialize_int(door->indice_amount, buf);
	while (i < door->indice_amount)
	{
		serialize_int(door->indices[i], buf);
		serialize_int(door->isquad[i], buf);
		k = 0;
		while (k < 4)
		{
			serialize_vec3(door->pos1[i][k], buf);
			serialize_vec3(door->pos2[i][k], buf);
			k++;
		}
		i++;
	}
	serialize_int(door->is_activation_pos_active, buf);
	serialize_vec3(door->activation_pos, buf);
	serialize_float(door->transition_time, buf);
}

void	deserialize_doors(t_level *level, t_buffer *buf)
{
	int	i;
	int	k;

	i = 0;
	while (i < level->doors.door_amount)
	{
		free(level->doors.door[i].indices);
		free(level->doors.door[i].isquad);
		k = 0;
		while (k < level->doors.door[i].indice_amount)
		{
			free(level->doors.door[i].pos1[k]);
			free(level->doors.door[i].pos2[k]);
			k++;
		}
		free(level->doors.door[i].pos1);
		free(level->doors.door[i].pos2);
		i++;
	}
	free(level->doors.door);
	deserialize_int(&level->doors.door_amount, buf);
	level->doors.door = (t_door *)malloc(sizeof(t_door)
			* level->doors.door_amount);
	if (!level->doors.door)
		ft_error("memory allocation failed\n");
	i = -1;
	while (++i < level->doors.door_amount)
		deserialize_door(&level->doors.door[i], buf);
}

void	serialize_doors(t_level *level, t_buffer *buf)
{
	int	i;

	serialize_int(level->doors.door_amount, buf);
	i = 0;
	while (i < level->doors.door_amount)
	{
		serialize_door(&level->doors.door[i], buf);
		i++;
	}
}

void	deserialize_lights(t_level *level, t_buffer *buf)
{
	int	i;

	deserialize_int(&level->light_amount, buf);
	level->lights = (t_light *)malloc(sizeof(t_light) * level->light_amount);
	if (!level->lights)
		ft_error("memory allocation failed\n");
	i = 0;
	while (i < level->light_amount)
	{
		deserialize_vec3(&level->lights[i].pos, buf);
		deserialize_color(&level->lights[i].color, buf);
		deserialize_float(&level->lights[i].radius, buf);
		deserialize_float(&level->lights[i].power, buf);
		i++;
	}
}

void	serialize_lights(t_level *level, t_buffer *buf)
{
	int	i;

	serialize_int(level->light_amount, buf);
	i = 0;
	while (i < level->light_amount)
	{
		serialize_vec3(level->lights[i].pos, buf);
		serialize_color(level->lights[i].color, buf);
		serialize_float(level->lights[i].radius, buf);
		serialize_float(level->lights[i].power, buf);
		i++;
	}
}

void	deserialize_level(t_level *level, t_buffer *buf)
{
	char	*str;
	int		i;

	str = deserialize_string(ft_strlen("doom-nukem"), buf);
	if (ft_strcmp(str, "doom-nukem"))
	{
		nonfatal_error("not valid doom-nukem map");
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
	free(level->spray.image);
	deserialize_bmp(&level->spray, buf);
	level->spray_overlay = (unsigned *)malloc(
			sizeof(unsigned) * level->texture.width * level->texture.height);
	if (!level->spray_overlay)
		ft_error("failed to allocate memory for file");
	i = 0;
	while (i < level->texture.height * level->texture.width)
	{
		deserialize_int((int *)&level->spray_overlay[i], buf);
		i++;
	}
	free_culling(level);
	free(level->all.tris);
	free(level->visible.tris);
	deserialize_obj(&level->all, buf);
	deserialize_doors(level, buf);
	deserialize_lights(level, buf);
	deserialize_float(&level->world_brightness, buf);
	deserialize_float(&level->skybox_brightness, buf);
	deserialize_player_pos(&level->game_logic.spawn_pos, buf);
	deserialize_int(&level->game_logic.health_box_amount, buf);
	level->game_logic.health_box = (t_item_pickup *)malloc(sizeof(t_item_pickup) * level->game_logic.health_box_amount);
	if (!level->game_logic.health_box)
		ft_error("memory allocation failed\n");
	i = -1;
	while (++i < level->game_logic.health_box_amount)
	{
		deserialize_vec3(&level->game_logic.health_box[i].pos, buf);
		level->game_logic.health_box[i].start_time = 0;
		level->game_logic.health_box[i].visible = 1;
	}
	deserialize_int(&level->game_logic.ammo_box_amount, buf);
	level->game_logic.ammo_box = (t_item_pickup *)malloc(sizeof(t_item_pickup) * level->game_logic.ammo_box_amount);
	if (!level->game_logic.ammo_box)
		ft_error("memory allocation failed\n");
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
	{
		deserialize_vec3(&level->game_logic.ammo_box[i].pos, buf);
		level->game_logic.ammo_box[i].start_time = 0;
		level->game_logic.ammo_box[i].visible = 1;
	}
	deserialize_int(&level->game_logic.enemy_amount, buf);
	level->game_logic.enemy_spawn_pos = (t_vec3 *)malloc(sizeof(t_vec3) * level->game_logic.enemy_amount);
	if (!level->game_logic.enemy_spawn_pos)
		ft_error("memory allocation failed\n");
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		deserialize_vec3(&level->game_logic.enemy_spawn_pos[i], buf);
	deserialize_int(&level->main_menu_anim.amount, buf);
	deserialize_int((int *)&level->main_menu_anim.duration, buf);
	deserialize_int(&level->main_menu_anim.loop, buf);
	level->main_menu_anim.pos = (t_player_pos *)malloc(sizeof(t_player_pos) * level->main_menu_anim.amount);
	if (!level->main_menu_anim.pos)
		ft_error("memory allocation failed\n");
	i = -1;
	while (++i < level->main_menu_anim.amount)
		deserialize_player_pos(&level->main_menu_anim.pos[i], buf);
	level->visible.tris = (t_tri *)malloc(sizeof(t_tri)
			* level->all.tri_amount);
	if (!level->visible.tris)
		ft_error("memory allocation failed\n");
	init_screen_space_partition(level);
	init_culling(level);
	level->level_initialized = TRUE;
}

void	serialize_level(t_level *level, t_buffer *buf)
{
	int	i;

	serialize_string("doom-nukem", buf);
	serialize_settings(level, buf);
	serialize_bmp(&level->texture, buf);
	serialize_bmp(&level->normal_map, buf);
	serialize_bmp(&level->sky.img, buf);
	serialize_bmp(&level->spray, buf);
	i = 0;
	while (i < level->texture.height * level->texture.width)
	{
		serialize_int(level->spray_overlay[i], buf);
		i++;
	}
	serialize_obj(&level->all, buf);
	serialize_doors(level, buf);
	serialize_lights(level, buf);
	serialize_float(level->world_brightness, buf);
	serialize_float(level->skybox_brightness, buf);
	serialize_player_pos(&level->game_logic.spawn_pos, buf);
	serialize_int(level->game_logic.health_box_amount, buf);
	i = -1;
	while (++i < level->game_logic.health_box_amount)
		serialize_vec3(level->game_logic.health_box[i].pos, buf);
	serialize_int(level->game_logic.ammo_box_amount, buf);
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
		serialize_vec3(level->game_logic.ammo_box[i].pos, buf);
	serialize_int(level->game_logic.enemy_amount, buf);
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		serialize_vec3(level->game_logic.enemy_spawn_pos[i], buf);
	serialize_int(level->main_menu_anim.amount, buf);
	serialize_int(level->main_menu_anim.duration, buf);
	serialize_int(level->main_menu_anim.loop, buf);
	i = -1;
	while (++i < level->main_menu_anim.amount)
		serialize_player_pos(&level->main_menu_anim.pos[i], buf);
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
	hFile = CreateFile(filename1, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		nonfatal_error("failed to create file");
	else
		WriteFile(hFile, buf->data, buf->next, &bytesWritten, NULL);
	free(filename1);
	CloseHandle(hFile);
}

void	open_file(char *filename, t_buffer *buf)
{
	HANDLE	hFile;
	DWORD	nRead;
	size_t	file_size;

	hFile = CreateFile(filename, GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		ft_error("failed to read file");
	file_size = (size_t)GetFileSize(hFile, NULL);
	buf->data = malloc(file_size + 1);
	if (!buf->data)
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
			S_IRGRP | S_IWGRP | S_IXGRP
			| S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd < 3)
		nonfatal_error("failed to create file");
	else if (write(fd, buf->data, buf->next) != buf->next)
		nonfatal_error("failed to write file");
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
	buf->data = malloc(buf->size + 1);
	if (!buf->data)
		ft_error("failed to allocate memory for file");
	if ((size_t)read(fd, buf->data, buf->size) != buf->size)
		ft_error("failed to read file");
	close(fd);
}
#endif

void	save_level(t_level *level)
{
	t_buffer	buf;

	if (!level->ui.state.save_filename[0]
		|| level->ui.state.save_filename[0] == '.')
	{
		nonfatal_error("Invalid filename");
		return ;
	}
	buf.data = malloc(SERIALIZE_INITIAL_BUFFER_SIZE);
	if (!buf.data)
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
	open_file(filename, &buf);
	deserialize_level(level, &buf);
	free(buf.data);
}
