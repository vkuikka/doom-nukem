/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deserialize.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 15:17:07 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/15 16:04:41 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serialization.h"

void	deserialize_vec2(t_vec2 *vec, t_buffer *buf)
{
	deserialize_float(&vec->x, buf);
	deserialize_float(&vec->y, buf);
}

void	deserialize_vec3(t_vec3 *vec, t_buffer *buf)
{
	deserialize_float(&vec->x, buf);
	deserialize_float(&vec->y, buf);
	deserialize_float(&vec->z, buf);
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

void	deserialize_vert(t_vert *vert, t_buffer *buf)
{
	deserialize_vec3(&vert->pos, buf);
	deserialize_vec2(&vert->txtr, buf);
}

void	deserialize_enemy_settings(t_enemy_settings *enemy, t_buffer *buf)
{
	deserialize_float(&enemy->dist_limit, buf);
	deserialize_float(&enemy->move_speed, buf);
	deserialize_float(&enemy->initial_health, buf);
	deserialize_float(&enemy->melee_range, buf);
	deserialize_float(&enemy->melee_damage, buf);
	deserialize_float(&enemy->attack_frequency, buf);
	deserialize_float(&enemy->move_duration, buf);
	deserialize_float(&enemy->shoot_duration, buf);
}

void	deserialize_projectile(t_projectile *projectile, t_buffer *buf)
{
	deserialize_float(&projectile->speed, buf);
	deserialize_float(&projectile->dist, buf);
	deserialize_float(&projectile->damage, buf);
	deserialize_float(&projectile->scale, buf);
}

void	deserialize_player_pos(t_player_pos *pos, t_buffer *buf)
{
	deserialize_vec3(&pos->pos, buf);
	deserialize_float(&pos->look_side, buf);
	deserialize_float(&pos->look_up, buf);
}

void	deserialize_perlin_settings(t_perlin_settings *p, t_buffer *buf)
{
	deserialize_float(&p->move_speed, buf);
	deserialize_float(&p->speed_diff, buf);
	deserialize_float(&p->scale, buf);
	deserialize_float(&p->min, buf);
	deserialize_float(&p->max, buf);
	deserialize_int(&p->resolution, buf);
	deserialize_float(&p->depth, buf);
	deserialize_float(&p->noise_opacity, buf);
	deserialize_float(&p->distance, buf);
	deserialize_float(&p->swirl, buf);
	deserialize_float(&p->swirl_interval, buf);
	deserialize_vec2(&p->dir, buf);
	deserialize_color(&p->color_1, buf);
	deserialize_color(&p->color_2, buf);
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
	deserialize_float(&tri->opacity, buf);
	deserialize_float(&tri->reflectivity, buf);
	deserialize_float(&tri->refractivity, buf);
	deserialize_int((int *)&tri->shader, buf);
	if (tri->shader == SHADER_PERLIN)
	{
		tri->perlin = (t_perlin_settings *)malloc(sizeof(t_perlin_settings));
		if (!tri->perlin)
			ft_error("failed to allocate memory for file");
		deserialize_perlin_settings(tri->perlin, buf);
	}
}

void	deserialize_obj(t_obj *obj, t_buffer *buf)
{
	int	i;

	i = 0;
	deserialize_int(&obj->tri_amount, buf);
	obj->tris = (t_tri *)malloc(sizeof(t_tri) * obj->tri_amount);
	if (!obj->tris)
		ft_error("failed to allocate memory for file (deserialize_obj)");
	ft_bzero(obj->tris, sizeof(t_tri) * obj->tri_amount);
	while (i < obj->tri_amount)
	{
		deserialize_tri(&obj->tris[i], buf);
		obj->tris[i].index = i;
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
		ft_error("failed to allocate memory for file (deserialize_bmp)");
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

char	*deserialize_string(int len, t_buffer *buf)
{
	char	*str;
	int		get;
	int		i;

	str = (char *)malloc(sizeof(char) * len + 1);
	if (!str)
		ft_error("memory allocation failed (deserialize_string)");
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

static void	malloc_door(t_door *door)
{
	door->indices = (int *)malloc(sizeof(int) * door->indice_amount);
	door->isquad = (int *)malloc(sizeof(int) * door->indice_amount);
	door->pos1 = (t_vec3 **)malloc(sizeof(t_vec3 *) * door->indice_amount);
	door->pos2 = (t_vec3 **)malloc(sizeof(t_vec3 *) * door->indice_amount);
	if (!door->pos1 || !door->pos2 || !door->isquad || !door->indices)
		ft_error("memory allocation failed (deserialize malloc_door)");
}

static void	deserialize_door_pos(t_door *door, t_buffer *buf, int door_index)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		deserialize_vec3(&door->pos1[door_index][i], buf);
		deserialize_vec3(&door->pos2[door_index][i], buf);
		i++;
	}
}

void	deserialize_door(t_door *door, t_buffer *buf)
{
	int	i;

	deserialize_int(&door->indice_amount, buf);
	malloc_door(door);
	i = 0;
	while (i < door->indice_amount)
	{
		deserialize_int(&door->indices[i], buf);
		deserialize_int(&door->isquad[i], buf);
		door->pos1[i] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
		door->pos2[i] = (t_vec3 *)malloc(sizeof(t_vec3) * 4);
		if (!door->pos1[i] || !door->pos2[i])
			ft_error("memory allocation failed (deserialize_door)");
		deserialize_door_pos(door, buf, i);
		i++;
	}
	deserialize_int(&door->is_activation_pos_active, buf);
	deserialize_vec3(&door->activation_pos, buf);
	deserialize_float(&door->transition_time, buf);
}

static void	free_doors(t_level *level)
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
}

void	deserialize_doors(t_level *level, t_buffer *buf)
{
	int	i;

	i = 0;
	free_doors(level);
	deserialize_int(&level->doors.door_amount, buf);
	level->doors.door = (t_door *)malloc(sizeof(t_door)
			* level->doors.door_amount);
	if (!level->doors.door)
		ft_error("memory allocation failed (deserialize doors)\n");
	i = -1;
	while (++i < level->doors.door_amount)
		deserialize_door(&level->doors.door[i], buf);
}

void	deserialize_lights(t_level *level, t_buffer *buf)
{
	int	i;

	deserialize_int(&level->light_amount, buf);
	level->lights = (t_light *)malloc(sizeof(t_light) * level->light_amount);
	if (!level->lights)
		ft_error("memory allocation failed (deserialize_lights)");
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

static void	deserialize_level_images(t_level *level, t_buffer *buf)
{
	int	i;

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
		ft_error("memory allocation failed (deserialize level, spray_overlay)");
	i = 0;
	while (i < level->texture.height * level->texture.width)
	{
		deserialize_int((int *)&level->spray_overlay[i], buf);
		i++;
	}
}

static void	deserialize_health_pickups(t_level *level, t_buffer *buf)
{
	int	i;

	deserialize_int(&level->game_logic.health_box_amount, buf);
	level->game_logic.health_box = (t_item_pickup *)malloc(
			sizeof(t_item_pickup) * level->game_logic.health_box_amount);
	if (!level->game_logic.health_box)
		ft_error("memory allocation failed (deserialize level, health)");
	i = -1;
	while (++i < level->game_logic.health_box_amount)
	{
		deserialize_vec3(&level->game_logic.health_box[i].pos, buf);
		level->game_logic.health_box[i].start_time = 0;
		level->game_logic.health_box[i].visible = 1;
	}
}

static void	deserialize_ammo_pickups(t_level *level, t_buffer *buf)
{
	int	i;

	deserialize_int(&level->game_logic.ammo_box_amount, buf);
	level->game_logic.ammo_box = (t_item_pickup *)malloc(
			sizeof(t_item_pickup) * level->game_logic.ammo_box_amount);
	if (!level->game_logic.ammo_box)
		ft_error("memory allocation failed (deserialize level, ammo)");
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
	{
		deserialize_vec3(&level->game_logic.ammo_box[i].pos, buf);
		level->game_logic.ammo_box[i].start_time = 0;
		level->game_logic.ammo_box[i].visible = 1;
	}
}

static void	deserialize_pickups(t_level *level, t_buffer *buf)
{
	deserialize_health_pickups(level, buf);
	deserialize_ammo_pickups(level, buf);
}

static void	deserialize_enemies(t_level *level, t_buffer *buf)
{
	int	i;

	deserialize_enemy_settings(&level->game_logic.enemy_settings, buf);
	deserialize_int(&level->game_logic.enemy_amount, buf);
	level->game_logic.enemies = (t_enemy *)malloc(
			sizeof(t_enemy) * level->game_logic.enemy_amount);
	if (!level->game_logic.enemies)
		ft_error("memory allocation failed (deserialize level, enemies)");
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		deserialize_vec3(&level->game_logic.enemies[i].spawn_pos, buf);
}

static void	deserialize_menu_anim(t_level *level, t_buffer *buf)
{
	int	i;

	deserialize_int(&level->main_menu_anim.amount, buf);
	deserialize_int((int *)&level->main_menu_anim.duration, buf);
	deserialize_int(&level->main_menu_anim.loop, buf);
	level->main_menu_anim.pos = (t_player_pos *)malloc(
			sizeof(t_player_pos) * level->main_menu_anim.amount);
	if (!level->main_menu_anim.pos)
		ft_error("memory allocation failed (deserialize level, menu anim pos)");
	i = -1;
	while (++i < level->main_menu_anim.amount)
		deserialize_player_pos(&level->main_menu_anim.pos[i], buf);
}

static int	check_file_header(t_buffer *buf)
{
	char	*str;

	str = deserialize_string(ft_strlen("doom-nukem"), buf);
	if (ft_strcmp(str, "doom-nukem"))
	{
		nonfatal_error("not valid doom-nukem map");
		free(str);
		return (1);
	}
	free(str);
	return (0);
}

void	deserialize_level(t_level *level, t_buffer *buf)
{
	if (check_file_header(buf))
		return ;
	deserialize_settings(level, buf);
	deserialize_level_images(level, buf);
	free_culling(level);
	free(level->all.tris);
	deserialize_projectile(&level->game_logic.player_projectile_settings, buf);
	deserialize_projectile(&level->game_logic.enemy_projectile_settings, buf);
	deserialize_obj(&level->all, buf);
	deserialize_doors(level, buf);
	deserialize_lights(level, buf);
	deserialize_float(&level->world_brightness, buf);
	deserialize_float(&level->skybox_brightness, buf);
	deserialize_player_pos(&level->game_logic.spawn_pos, buf);
	deserialize_pickups(level, buf);
	deserialize_enemies(level, buf);
	deserialize_menu_anim(level, buf);
	init_culling(level);
	level->level_initialized = TRUE;
}
