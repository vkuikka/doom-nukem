/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serialize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/06 14:13:02 by rpehkone          #+#    #+#             */
/*   Updated: 2021/10/15 15:58:10 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serialization.h"

void	serialize_vec2(t_vec2 vec, t_buffer *buf)
{
	serialize_float(vec.x, buf);
	serialize_float(vec.y, buf);
}

void	serialize_vec3(t_vec3 vec, t_buffer *buf)
{
	serialize_float(vec.x, buf);
	serialize_float(vec.y, buf);
	serialize_float(vec.z, buf);
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

void	serialize_vert(t_vert *vert, t_buffer *buf)
{
	serialize_vec3(vert->pos, buf);
	serialize_vec2(vert->txtr, buf);
}

void	serialize_enemy_settings(t_enemy_settings *enemy, t_buffer *buf)
{
	serialize_float(enemy->dist_limit, buf);
	serialize_float(enemy->move_speed, buf);
	serialize_float(enemy->initial_health, buf);
	serialize_float(enemy->melee_range, buf);
	serialize_float(enemy->melee_damage, buf);
	serialize_float(enemy->attack_frequency, buf);
	serialize_float(enemy->move_duration, buf);
	serialize_float(enemy->shoot_duration, buf);
}

void	serialize_projectile(t_projectile *projectile, t_buffer *buf)
{
	serialize_float(projectile->speed, buf);
	serialize_float(projectile->dist, buf);
	serialize_float(projectile->damage, buf);
	serialize_float(projectile->scale, buf);
}

void	serialize_player_pos(t_player_pos *pos, t_buffer *buf)
{
	serialize_vec3(pos->pos, buf);
	serialize_float(pos->look_side, buf);
	serialize_float(pos->look_up, buf);
}

void	serialize_perlin_settings(t_perlin_settings *p, t_buffer *buf)
{
	serialize_float(p->move_speed, buf);
	serialize_float(p->speed_diff, buf);
	serialize_float(p->scale, buf);
	serialize_float(p->min, buf);
	serialize_float(p->max, buf);
	serialize_int(p->resolution, buf);
	serialize_float(p->depth, buf);
	serialize_float(p->noise_opacity, buf);
	serialize_float(p->distance, buf);
	serialize_float(p->swirl, buf);
	serialize_float(p->swirl_interval, buf);
	serialize_vec2(p->dir, buf);
	serialize_color(p->color_1, buf);
	serialize_color(p->color_2, buf);
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
	serialize_float(tri->opacity, buf);
	serialize_float(tri->reflectivity, buf);
	serialize_float(tri->refractivity, buf);
	serialize_int((int)tri->shader, buf);
	if (tri->shader == SHADER_PERLIN)
		serialize_perlin_settings(tri->perlin, buf);
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

static void	serialize_level_images(t_level *level, t_buffer *buf)
{
	int	i;

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
}

static void	serialize_pickups(t_level *level, t_buffer *buf)
{
	int	i;

	serialize_int(level->game_logic.health_box_amount, buf);
	i = -1;
	while (++i < level->game_logic.health_box_amount)
		serialize_vec3(level->game_logic.health_box[i].pos, buf);
	serialize_int(level->game_logic.ammo_box_amount, buf);
	i = -1;
	while (++i < level->game_logic.ammo_box_amount)
		serialize_vec3(level->game_logic.ammo_box[i].pos, buf);
}

static void	serialize_enemies(t_level *level, t_buffer *buf)
{
	int	i;

	serialize_enemy_settings(&level->game_logic.enemy_settings, buf);
	serialize_int(level->game_logic.enemy_amount, buf);
	i = -1;
	while (++i < level->game_logic.enemy_amount)
		serialize_vec3(level->game_logic.enemies[i].spawn_pos, buf);
}

static void	serialize_menu_anim(t_level *level, t_buffer *buf)
{
	int	i;

	serialize_int(level->main_menu_anim.amount, buf);
	serialize_int(level->main_menu_anim.duration, buf);
	serialize_int(level->main_menu_anim.loop, buf);
	i = -1;
	while (++i < level->main_menu_anim.amount)
		serialize_player_pos(&level->main_menu_anim.pos[i], buf);
}

void	serialize_level(t_level *level, t_buffer *buf)
{
	serialize_string("doom-nukem", buf);
	serialize_settings(level, buf);
	serialize_level_images(level, buf);
	serialize_projectile(&level->game_logic.player_projectile_settings, buf);
	serialize_projectile(&level->game_logic.enemy_projectile_settings, buf);
	serialize_obj(&level->all, buf);
	serialize_doors(level, buf);
	serialize_lights(level, buf);
	serialize_float(level->world_brightness, buf);
	serialize_float(level->skybox_brightness, buf);
	serialize_player_pos(&level->game_logic.spawn_pos, buf);
	serialize_pickups(level, buf);
	serialize_enemies(level, buf);
	serialize_menu_anim(level, buf);
}
