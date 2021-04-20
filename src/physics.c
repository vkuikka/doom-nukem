/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 01:23:16 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/18 13:12:46 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static float	        cast_all(t_ray vec, t_level *level, float *dist_u, float *dist_d, int *index)
{
	float	res = FLT_MAX;

	vec_normalize(&vec.dir);
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		float tmp;
		tmp = cast_face(level->all.tris[i], vec, NULL);
		if (dist_u != NULL)
		{
			if (tmp > 0 && tmp < *dist_d)
				*dist_d = tmp;
			else if (tmp < 0 && tmp > *dist_u)
				*dist_u = tmp;
		}
		else if (tmp > 0 && tmp < res)
		{
			res = tmp;
			if (index)
				*index = i;
		}
	}
	return (res);
}

static void			player_input(t_level *level, t_vec3 *wishdir, float *shift)
{
	const Uint8		*keys = SDL_GetKeyboardState(NULL);

	ft_bzero(wishdir, sizeof(t_vec3));
	if (level->ui.state.text_input_enable)
		return ;
	if (keys[SDL_SCANCODE_W])
		wishdir->z += 1;
	if (keys[SDL_SCANCODE_S])
		wishdir->z -= 1;
	if (keys[SDL_SCANCODE_A])
		wishdir->x -= 1;
	if (keys[SDL_SCANCODE_D])
		wishdir->x += 1;
	if (keys[SDL_SCANCODE_LEFT])
		level->cam.look_side -= 0.004;
	if (keys[SDL_SCANCODE_RIGHT])
		level->cam.look_side += 0.004;
	if (keys[SDL_SCANCODE_SPACE])
		wishdir->y -= 1;
	if (keys[SDL_SCANCODE_LSHIFT] && level->ui.noclip)
		wishdir->y += 1;
	*shift = 1;
	if (keys[SDL_SCANCODE_LSHIFT] && !level->ui.noclip)
		*shift = .5;
}

static void	        player_collision(t_vec3 *vel, t_vec3 *pos, t_level *level)
{
	t_ray			r;
	float			dist = 0;
	int				index;

	r.pos.x = pos->x;
	r.pos.y = pos->y;
	r.pos.z = pos->z;
	r.dir.x = vel->x;
	r.dir.y = vel->y;
	r.dir.z = vel->z;
	dist = cast_all(r, level, NULL, NULL, &index);
	if (dist > 0 && dist <= vec_length(*vel) + WALL_CLIP_DIST)
	{
		t_vec3	normal;
		vec_cross(&normal, level->all.tris[index].v0v1, level->all.tris[index].v0v2);
		vec_normalize(&normal);
		vec_mult(&normal, vec_dot(*vel, normal));
		vec_sub(vel, *vel, normal);
	}
}

static int				is_player_in_air(t_level *level)
{
	float	dist;
	t_ray	r;

	r.pos.x = level->cam.pos.x;
	r.pos.y = level->cam.pos.y;
	r.pos.z = level->cam.pos.z;
	r.dir.x = 0;
	r.dir.y = 1;
	r.dir.z = 0;
	dist = cast_all(r, level, NULL, NULL, NULL);
	if (dist > 0 && dist < PLAYER_HEIGHT + .02 && !level->ui.noclip)
	{
		if (dist < PLAYER_HEIGHT)
			level->cam.pos.y -= PLAYER_HEIGHT - dist;
		return (FALSE);
	}
	else
		return (TRUE);
}

static void	noclip(t_level *level, t_vec3 *wishdir, t_vec3 *vel, float delta_time)
{
	if (wishdir->x && wishdir->y && wishdir->z)
		vec_normalize(wishdir);
	vec_mult(wishdir, NOCLIP_SPEED);
	level->cam.pos.x += wishdir->x * delta_time;
	level->cam.pos.y += wishdir->y * delta_time;
	level->cam.pos.z += wishdir->z * delta_time;
	ft_bzero(vel, sizeof(t_vec3));
	level->ui.horizontal_velocity = vec_length(*wishdir);
}

static void	        rotate_wishdir(t_level *level, t_vec3 *wishdir, t_vec3 *vel)
{
	if (wishdir->x && wishdir->z)
	{
		float w = sqrt(wishdir->x * wishdir->x + wishdir->z * wishdir->z);
		wishdir->x /= w;
		wishdir->z /= w;
	}
	rotate_vertex(level->cam.look_side, wishdir, 0);
}

void		vertical_movement(t_vec3 *wishdir, t_vec3 *vel, float delta_time, int in_air)
{
	if (in_air)
	{
		wishdir->y = GRAVITY;
		vel->y += wishdir->y * delta_time;
	}
	else
	{
		if (vel->y > 0)
			vel->y = 0;
		vel->y = wishdir->y * JUMP_SPEED;
	}
}

void		air_movement(t_vec3 *wishdir, t_vec3 *vel, float delta_time)
{
	if (wishdir->x || wishdir->z)
	{
		float length = sqrt(wishdir->x * wishdir->x + wishdir->z * wishdir->z);
		wishdir->x /= length;
		wishdir->z /= length;
		float speed = fmax(AIR_ACCEL - (vel->x * wishdir->x + vel->z * wishdir->z), 0);
		wishdir->x *= speed;
		wishdir->z *= speed;
		vel->x += wishdir->x;
		vel->z += wishdir->z;
	}
}

void		horizontal_movement(t_vec3 *wishdir, t_vec3 *vel, float delta_time, float shift)
{
	if (wishdir->x || wishdir->z)
	{
		vel->x += wishdir->x * MOVE_ACCEL * delta_time;
		vel->z += wishdir->z * MOVE_ACCEL * delta_time;
		float speed;
		if ((speed = sqrtf(vel->x * vel->x + vel->z * vel->z)) > MOVE_SPEED * shift)
		{
			vel->x *= MOVE_SPEED * shift / speed;
			vel->z *= MOVE_SPEED * shift / speed;
		}
	}
	else
	{
		if (fabs(vel->x * GROUND_FRICTION * delta_time) > fabs(vel->x) ||
			fabs(vel->z * GROUND_FRICTION * delta_time) > fabs(vel->z))
		{
			vel->x = 0;
			vel->z = 0;
		}
		else
		{
			vel->x -= vel->x * GROUND_FRICTION * delta_time;
			vel->z -= vel->z * GROUND_FRICTION * delta_time;
		}
	}
}

void	        player_movement(t_level *level)
{
	static t_vec3	vel = {0, 0, 0};
	int				in_air;
	t_vec3			wishdir;
	float			shift;
	float			delta_time;

	delta_time = level->ui.frametime / 1000.;
	player_input(level, &wishdir, &shift);
	rotate_wishdir(level, &wishdir, &vel);
	if (level->ui.noclip)
		return (noclip(level, &wishdir, &vel, delta_time));
	in_air = is_player_in_air(level);
	vertical_movement(&wishdir, &vel, delta_time, in_air);
	if (in_air || wishdir.y)
		air_movement(&wishdir, &vel, delta_time);
	else
		horizontal_movement(&wishdir, &vel, delta_time, shift);
	if (vel.x || vel.y || vel.z)
	{
		vec_mult(&vel, delta_time);
		player_collision(&vel, &level->cam.pos, level);
		level->cam.pos.x += vel.x;
		level->cam.pos.y += vel.y;
		level->cam.pos.z += vel.z;
		vec_div(&vel, delta_time);
		level->ui.horizontal_velocity = sqrt(vel.x * vel.x + vel.z * vel.z);
	}
}
