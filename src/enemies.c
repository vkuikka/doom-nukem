/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enemies.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/14 17:08:49 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/12 16:33:29by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void			create_projectile(t_level *level, t_vec3 pos, t_vec3 dir, t_enemy *enemy)
{
	int		index;

	index = level->all.tri_amount;
	free_culling(level);
	level->all.tri_amount++;
	if (!(level->all.tris = (t_tri*)realloc(level->all.tris, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	if (!(level->visible.tris = (t_tri*)realloc(level->visible.tris, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	set_new_face(level, pos, dir, enemy->projectile_scale);
	init_screen_space_partition(level);
	init_culling(level);
	if (!(level->all.tris[index].projectile = (t_projectile*)malloc(sizeof(t_projectile))))
		ft_error("memory allocation failed");
	level->all.tris[index].projectile->dist = enemy->attack_range;
	level->all.tris[index].projectile->damage = enemy->attack_damage;
	level->all.tris[index].projectile->speed = enemy->projectile_speed;
	level->all.tris[index].projectile->dir = dir;
	level->all.tris[index].isprojectile = 1;
	level->all.tris[index].verts[0].txtr = enemy->projectile_uv[0];
	level->all.tris[index].verts[1].txtr = enemy->projectile_uv[1];
	level->all.tris[index].verts[2].txtr = enemy->projectile_uv[2];
}

static void		remove_projectile(t_level *level, int remove)
{
	int		index;
	t_tri	*new_tris;

	index = level->all.tri_amount;
	free_culling(level);
	level->all.tri_amount--;
	if (!(new_tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	if (!(level->visible.tris = (t_tri*)realloc(level->visible.tris, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	free(level->all.tris[remove].projectile);
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (i < remove)
			new_tris[i] = level->all.tris[i];
		else
		{
			new_tris[i] = level->all.tris[i + 1];
			new_tris[i].index = i;
		}
	}
	free(level->all.tris);
	level->all.tris = new_tris;
	init_screen_space_partition(level);
	init_culling(level);
}

static void		calc_vectors(t_tri *tri)
{
	tri->v0v1.x = tri->verts[2].pos.x - tri->verts[0].pos.x;
	tri->v0v1.y = tri->verts[2].pos.y - tri->verts[0].pos.y;
	tri->v0v1.z = tri->verts[2].pos.z - tri->verts[0].pos.z;
	tri->v0v2.x = tri->verts[1].pos.x - tri->verts[0].pos.x;
	tri->v0v2.y = tri->verts[1].pos.y - tri->verts[0].pos.y;
	tri->v0v2.z = tri->verts[1].pos.z - tri->verts[0].pos.z;
}

static float	find_angle(t_vec3 v1, t_vec3 v2)
{
	float	angle;

	angle = M_PI - vec_angle(v1, v2);
	if (isnan(angle))
		return (0);
	rotate_vertex(M_PI / 2, &v2, 0);
	angle *= vec_dot(v1, v2) < 0 ? -1 : 1;
	return (angle);
}

static void		turn_sprite(t_tri *tri, t_vec3 dir)
{
	t_vec3	face_mid;
	t_vec3	rot_vert;
	t_vec3	normal;
	float	angle;
	int		vert;

	vert = 0;
	vec_cross(&normal, tri->v0v1, tri->v0v2);
	normal.y = 0;
	ft_memset(&face_mid, 0, sizeof(int) * 3);
	while (vert < 3 + tri->isquad)
		vec_add(&face_mid, face_mid, tri->verts[vert++].pos);
	vec_div(&face_mid, (float)vert);
	vec_sub(&rot_vert, dir, face_mid);
	rot_vert.y = 0;
	if (!(angle = find_angle(normal, rot_vert)))
		return;
	while (vert--)
	{
		vec_sub(&rot_vert, tri->verts[vert].pos, face_mid);
		rotate_vertex(angle, &rot_vert, 0);
		vec_add(&tri->verts[vert].pos, rot_vert, face_mid);
	}
	calc_vectors(tri);
	vec_cross(&tri->normal, tri->v0v2, tri->v0v1);
	vec_normalize(&tri->normal);
}

void			move_enemy(t_tri *face, t_level *level, float time)
{
	t_ray	e;
	float	dist;
	t_vec3	player;
	t_vec3	tmp;

	player = level->cam.pos;
	e.pos.x = 0;
	e.pos.y = 0;
	e.pos.z = 0;
	for (int i = 0; i < 3 + face->isquad; i++)
		vec_add(&e.pos, e.pos, face->verts[i].pos);
	vec_div(&e.pos, 3 + face->isquad);
	vec_sub(&e.dir, player, e.pos);
	dist = cast_all(e, level, NULL, NULL, NULL);
	if (player.y > e.pos.y - ENEMY_MOVABLE_HEIGHT_DIFF &&
		player.y < e.pos.y + ENEMY_MOVABLE_HEIGHT_DIFF)
	{
		if (dist > vec_length(e.dir))
			face->enemy->dir = e.dir;
		if ((dist > vec_length(e.dir) && vec_length(e.dir) > face->enemy->dist_limit) || dist < vec_length(e.dir) - face->enemy->dist_limit)
		{
			if (face->enemy->dir.x || face->enemy->dir.z)
			{
				e.dir = face->enemy->dir;
				e.dir.y = 0;
				vec_normalize(&e.dir);
				vec_mult(&e.dir, face->enemy->move_speed * time);
				tmp = e.dir;
				vec_add(&e.pos, e.pos, e.dir);
				vec_sub(&tmp, face->enemy->dir, e.dir);
				if (vec_dot(tmp, e.dir) > 0)
				{
					face->enemy->dir = tmp;
					for (int i = 0; i < 3 + face->isquad; i++)
						vec_add(&face->verts[i].pos, face->verts[i].pos, e.dir);
				}
			}
		}
	}
	vec_sub(&tmp, player, e.pos);
	face->enemy->current_attack_delay += time;
	if (dist > vec_length(tmp) && face->enemy->current_attack_delay >= face->enemy->attack_frequency)
	{
		if (face->enemy->projectile_speed)
		{
			face->enemy->current_attack_delay = 0;
			vec_sub(&e.dir, player, e.pos);
			vec_normalize(&e.dir);
			create_projectile(level, e.pos, e.dir, face->enemy);
		}
		if (vec_length(tmp) < face->enemy->attack_range)
		{
			face->enemy->current_attack_delay = 0;
			vec_mult(&level->player_vel, 0);
			level->player_health -= face->enemy->attack_damage;
		}
	}
}

static void		move_projectile(t_tri *face, t_level *level, float time)
{
	t_ray	e;
	float	dist;
	t_vec3	player;

	player = level->cam.pos;
	e.pos = face->verts[0].pos;
	vec_add(&e.pos, e.pos, face->v0v1);
	vec_add(&e.pos, e.pos, face->v0v2);
	vec_avg(&e.pos, e.pos, face->verts[0].pos);
	vec_sub(&e.dir, player, e.pos);
	if (vec_length(e.dir) <= 3)
	{
		vec_mult(&level->player_vel, 0);
		level->player_health -= face->projectile->damage;
		remove_projectile(level, face->index);
		return;
	}
	e.dir = face->projectile->dir;
	dist = cast_all(e, level, NULL, NULL, NULL);
	vec_mult(&e.dir, face->projectile->speed * time);
	if (dist <= vec_length(e.dir) || face->projectile->dist > MAX_PROJECTILE_TRAVEL)
	{
		remove_projectile(level, face->index);
		return;
	}
	for (int i = 0; i < 3 + face->isquad; i++)
		vec_add(&face->verts[i].pos, face->verts[i].pos, e.dir);
	face->projectile->dist += vec_length(e.dir);
}

void			enemies_update_sprites(t_level *level)
{
	static int	time = 0;
	int			delta_time;
	int			face;

	face = 0;
	delta_time = SDL_GetTicks() - time;
	while (face < level->all.tri_amount)
	{
		if (level->all.tris[face].isenemy)
		{
			turn_sprite(&level->all.tris[face], level->cam.pos);
			move_enemy(&level->all.tris[face], level, delta_time / 1000.0);
		}
		else if (level->all.tris[face].isprojectile)
		{
			turn_sprite(&level->all.tris[face], level->cam.pos);
			move_projectile(&level->all.tris[face], level, delta_time / 1000.0);
		}
		face++;
	}
	time = SDL_GetTicks();
}
