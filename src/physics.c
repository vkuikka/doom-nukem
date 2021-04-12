/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 01:23:16 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/10 02:44:34 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

float	        cast_all(t_ray vec, t_level *level, float *dist_u, float *dist_d, int *index)
{
	float	res = FLT_MAX;

	vec_normalize(&vec.dir);
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		float tmp;
		tmp = cast_face(level->all.tris[i], vec, NULL, NULL);
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

t_vec3	        player_input(int noclip, t_level *level, int in_air, t_vec3 vel)
{
	const Uint8		*keys = SDL_GetKeyboardState(NULL);
	t_vec3			wishdir;
	float			speed;

	wishdir.x = 0;
	wishdir.y = 0;
	wishdir.z = 0;
	if (level->ui.state.text_input_enable)
		return (wishdir);
	speed = 0;
	if (keys[SDL_SCANCODE_LSHIFT] && noclip)
		wishdir.y += 1;
	if (keys[SDL_SCANCODE_SPACE] && (!in_air || noclip))
		wishdir.y -= 1;
	if (keys[SDL_SCANCODE_W])
		wishdir.z += 1;
	if (keys[SDL_SCANCODE_S])
		wishdir.z -= 1;
	if (keys[SDL_SCANCODE_A])
		wishdir.x -= 1;
	if (keys[SDL_SCANCODE_D])
		wishdir.x += 1;

	if (keys[SDL_SCANCODE_LEFT])
		level->cam.look_side -= 0.004;
	if (keys[SDL_SCANCODE_RIGHT])
		level->cam.look_side += 0.004;

	if (wishdir.x || wishdir.z)
	{
		rotate_vertex(level->cam.look_side, &wishdir, 0);
		float length = sqrt(wishdir.x * wishdir.x + wishdir.z * wishdir.z);
		wishdir.x /= length;
		wishdir.z /= length;
	}
	if (noclip)
		speed = NOCLIP_SPEED;
	else
		speed = fmax(MOVE_SPEED - (vel.x * wishdir.x + vel.z * wishdir.z), 0);

	wishdir.x *= speed;
	wishdir.z *= speed;
	wishdir.y *= JUMP_SPEED;	//horizontal velocity does not affect vertical velocity

	if (keys[SDL_SCANCODE_LSHIFT] && !noclip)
		vec_mult(&wishdir, 0.5);

	return (wishdir);
}

void	        player_collision(t_vec3 *vel, t_vec3 *pos, t_level *level)
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
		t_vec3	clip;
		vec_sub(&clip, *vel, normal);
		vel->x = clip.x;
		vel->y = clip.y;
		vel->z = clip.z;
	}
}

int				is_player_in_air(t_level *level)
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
	if (dist > 0 && dist < PLAYER_HEIGHT + GRAVITY && !level->ui.noclip)
	{
		if (dist < PLAYER_HEIGHT)
			level->cam.pos.y -= PLAYER_HEIGHT - dist;
		return (FALSE);
	}
	else
		return (TRUE);
}

void	        player_movement(t_level *level)
{
	static t_vec3	vel = {0, 0, 0};
	int				in_air;
	t_vec3			*pos;
	t_vec3			wishdir;

	pos = &level->cam.pos;
	in_air = is_player_in_air(level);
	wishdir = player_input(level->ui.noclip, level, in_air, vel);
	vel.y = fmax(fmin(vel.y, 0.5), -0.5);

	if (level->ui.noclip)
	{
		pos->x += wishdir.x;
		pos->y += wishdir.y;
		pos->z += wishdir.z;
		vel.x = 0;
		vel.y = 0;
		vel.z = 0;
		return ;
	}
	if (vel.x || vel.y || vel.z)
		player_collision(&vel, pos, level);
	pos->x += vel.x;
	pos->y += vel.y;
	pos->z += vel.z;
	if ((wishdir.x || wishdir.y || wishdir.z)) //&& sqrtf(vel.x * vel.x + vel.z * vel.z) < MAX_SPEED)
	{
		vel.x += wishdir.x;
		vel.y += wishdir.y;
		vel.z += wishdir.z;
	}
	else if (!in_air && wishdir.x == 0 && wishdir.z == 0)
	{
		vel.x *= 0.9;
		vel.z *= 0.9;
	}
	if (in_air)
		vel.y += GRAVITY;
	else if (vel.y > 0)
		vel.y = 0;
}
