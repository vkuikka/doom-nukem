/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   physics.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/10 01:23:16 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/02 19:15:42 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	player_movement_input(const Uint8 *keys, t_level *level,
				t_vec3 *wishdir, float *height)
{
	if (keys[SDL_SCANCODE_W])
		wishdir->z += 1;
	if (keys[SDL_SCANCODE_S])
		wishdir->z -= 1;
	if (keys[SDL_SCANCODE_A])
		wishdir->x -= 1;
	if (keys[SDL_SCANCODE_D])
		wishdir->x += 1;
	if (keys[SDL_SCANCODE_SPACE])
		wishdir->y -= 1;
	if (keys[SDL_SCANCODE_LSHIFT] && level->ui.noclip)
		wishdir->y += 1;
	if (keys[SDL_SCANCODE_LCTRL] && !level->ui.noclip)
	{
		*height = CROUCHED_HEIGHT;
		level->player.move_speed = CROUCH_SPEED;
	}
	else if (keys[SDL_SCANCODE_LSHIFT] && !level->ui.noclip)
		level->player.move_speed = WALK_SPEED;
	else if (!level->ui.noclip)
		level->player.move_speed = RUN_SPEED;
}

static void	player_input(t_level *level, t_vec3 *wishdir, float *height)
{
	const Uint8	*keys;

	ft_bzero(wishdir, sizeof(t_vec3));
	if (level->ui.state.text_input_enable)
		return ;
	keys = SDL_GetKeyboardState(NULL);
	player_movement_input(keys, level, wishdir, height);
	if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
	{
		if (keys[SDL_SCANCODE_LEFT])
			level->ui.state.uv_pos.x -= 10;
		if (keys[SDL_SCANCODE_RIGHT])
			level->ui.state.uv_pos.x += 10;
		if (keys[SDL_SCANCODE_UP])
			level->ui.state.uv_pos.y -= 10;
		if (keys[SDL_SCANCODE_DOWN])
			level->ui.state.uv_pos.y += 10;
		if (keys[SDL_SCANCODE_MINUS] && level->ui.state.uv_zoom > 0.5)
			level->ui.state.uv_zoom -= 0.01;
		if (keys[SDL_SCANCODE_EQUALS] && level->ui.state.uv_zoom < 10)
			level->ui.state.uv_zoom += 0.01;
	}
}

// first if player to ground
// second if is not floor, is in clip distance
static int	player_collision(t_vec3 *vel, t_vec3 *pos, t_level *level,
															float height)
{
	t_ray	r;
	float	dist;
	int		index;
	t_vec3	normal;

	dist = 0;
	r.pos = *pos;
	r.dir = (t_vec3){0, 1, 0};
	dist = cast_all(r, level, &index);
	if (dist != FLT_MAX && dist <= height)
		pos->y -= height - dist;
	r.pos = *pos;
	r.pos.y += height / PLAYER_HEIGHT_MAGIC;
	r.dir = *vel;
	dist = cast_all(r, level, &index);
	if (dist != FLT_MAX
		&& - level->all.tris[index].normal.y < WALKABLE_NORMAL_MIN_Y
		&& dist <= vec_length(*vel) + WALL_CLIP_DIST)
	{
		normal = level->all.tris[index].normal;
		vec_mult(&normal, vec_dot(*vel, normal));
		vec_sub(vel, *vel, normal);
		return (1);
	}
	return (0);
}

int	is_player_in_air(t_level *level, float height)
{
	float	dist;
	int		index;
	t_ray	r;

	r.pos.x = level->cam.pos.x;
	r.pos.y = level->cam.pos.y;
	r.pos.z = level->cam.pos.z;
	r.dir.x = 0;
	r.dir.y = 1;
	r.dir.z = 0;
	dist = cast_all(r, level, &index);
	if (dist < height + .02
		&& - level->all.tris[index].normal.y > WALKABLE_NORMAL_MIN_Y
		&& !level->ui.noclip)
	{
		return (FALSE);
	}
	else
		return (TRUE);
}

static void	noclip(t_level *level, t_vec3 *wishdir, float delta_time)
{
	if (wishdir->x && wishdir->y && wishdir->z)
		vec_normalize(wishdir);
	vec_mult(wishdir, NOCLIP_SPEED);
	level->cam.pos.x += wishdir->x * delta_time;
	level->cam.pos.y += wishdir->y * delta_time;
	level->cam.pos.z += wishdir->z * delta_time;
	level->ui.horizontal_velocity = vec_length(*wishdir);
	level->player_vel = (t_vec3){0, 0, 0};
}

static void	rotate_wishdir(t_level *level, t_vec3 *wishdir)
{
	float	w;

	if (wishdir->x && wishdir->z)
	{
		w = sqrt(wishdir->x * wishdir->x + wishdir->z * wishdir->z);
		wishdir->x /= w;
		wishdir->z /= w;
	}
	rotate_vertex(level->cam.look_side, wishdir, 0);
}

static int	vertical_movement(t_vec3 *wishdir, t_vec3 *vel, float delta_time,
																	int in_air)
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
		if (wishdir->y == -1)
			return (1);
	}
	return (0);
}

static void	air_movement(t_vec3 *wishdir, t_vec3 *vel)
{
	float	length;
	float	speed;

	if (wishdir->x || wishdir->z)
	{
		length = sqrt(wishdir->x * wishdir->x + wishdir->z * wishdir->z);
		wishdir->x /= length;
		wishdir->z /= length;
		speed = fmax(AIR_ACCEL
				- (vel->x * wishdir->x + vel->z * wishdir->z), 0);
		wishdir->x *= speed;
		wishdir->z *= speed;
		vel->x += wishdir->x;
		vel->z += wishdir->z;
	}
}

static void	horizontal_movement(t_vec3 *wishdir, t_vec3 *vel,
								float delta_time, float movespeed)
{
	float	speed;

	if (wishdir->x || wishdir->z)
	{
		vel->x += wishdir->x * MOVE_ACCEL * delta_time;
		vel->z += wishdir->z * MOVE_ACCEL * delta_time;
		speed = sqrtf(vel->x * vel->x + vel->z * vel->z);
		if (speed > movespeed)
		{
			vel->x *= movespeed / speed;
			vel->z *= movespeed / speed;
		}
	}
	else
	{
		if (fabs(vel->x * GROUND_FRICTION * delta_time) > fabs(vel->x)
			|| fabs(vel->z * GROUND_FRICTION * delta_time) > fabs(vel->z))
			vel = &((t_vec3){0, vel->y, 0});
		else
		{
			vel->x -= vel->x * GROUND_FRICTION * delta_time;
			vel->z -= vel->z * GROUND_FRICTION * delta_time;
		}
	}
}

static void	movement_physics(t_level *level, t_vec3 wishdir,
					float height, float delta_time)
{
	t_vec3	vel;
	int		in_air;

	vel = level->player_vel;
	in_air = is_player_in_air(level, height);
	vertical_movement(&wishdir, &vel, delta_time, in_air);
	if (in_air || wishdir.y)
		air_movement(&wishdir, &vel);
	else
		horizontal_movement(&wishdir, &vel, delta_time,
			level->player.move_speed);
	if (vel.x || vel.y || vel.z)
	{
		vec_mult(&vel, delta_time);
		while (player_collision(&vel, &level->cam.pos, level, height))
			;
		vec_add(&level->cam.pos, level->cam.pos, vel);
		vec_div(&vel, delta_time);
		level->ui.horizontal_velocity = sqrt(vel.x * vel.x + vel.z * vel.z);
	}
	level->player_vel = vel;
}

void	player_movement(t_level *level)
{
	t_vec3	wishdir;
	float	delta_time;
	float	height;

	height = PLAYER_HEIGHT;
	delta_time = level->ui.frame_time / 1000.;
	player_input(level, &wishdir, &height);
	rotate_wishdir(level, &wishdir);
	if (level->ui.noclip)
		return (noclip(level, &wishdir, delta_time));
	movement_physics(level, wishdir, height, delta_time);
}
