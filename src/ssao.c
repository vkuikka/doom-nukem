/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ssao.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 14:38:16 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/20 23:22:04 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

t_vec3	px_to_vec(t_camera *cam, int x, int y)
{
	float	xm;
	float	ym;
	t_vec3	result;

	xm = cam->fov_x;
	ym = cam->fov_y;
	xm = xm / RES_X * x - xm / 2;
	ym = ym / RES_Y * y - ym / 2;
	result.x = cam->front.x + cam->up.x * ym + cam->side.x * xm;
	result.y = cam->front.y + cam->up.y * ym + cam->side.y * xm;
	result.z = cam->front.z + cam->up.z * ym + cam->side.z * xm;
	return (result);
}

static float	surrounding_diff(int x, int y, t_level *level, t_window *win)
{
	float	avg;
	float	val;
	int		count;
	int		radius = level->ui.ssao_radius * level->ui.raycast_quality;
	t_ivec2	i;
	t_ivec2	start;

	start.x = x - radius;
	start.y = y - radius;
	count = 0;
	avg = 0;
	i.x = start.x;
	while (i.x < x + radius)
	{
		i.y = start.y;
		while (i.y < y + radius)
		{
			if (i.x < RES_X && i.y < RES_Y && i.x > 0 && i.y > 0)
			{
				// t_vec3	v1;
				// t_vec3	v2;
				// v1 = px_to_vec(&level->cam, x, y);
				// v2 = px_to_vec(&level->cam, i.x, i.y);
				// vec_normalize(&v1);
				// vec_normalize(&v2);
				// vec_mult(&v1, win->depth_buffer[x + y * RES_X]);
				// vec_mult(&v2, win->depth_buffer[i.x + i.y * RES_X]);
				// float	v1_to_v2;
				// vec_sub(&v1, v2, v1);
				// v1_to_v2 = vec_dot(v1, win->normal_buffer[i.x + i.y * RES_X]);

				val = vec_dot(win->normal_buffer[x + y * RES_X], win->normal_buffer[i.x + i.y * RES_X]);
				float dist = win->depth_buffer[i.x + i.y * RES_X];
				// float dist2 = win->depth_buffer[x + y * RES_X];
				if (dist != FLT_MAX)
				if (val > -0.1)
				{
					val = 1 - fabs(val);
					val *= circle_gradient(i, start, radius * 2);
					avg += val;
					count++;
				}
			}
			i.y += level->ui.raycast_quality;
		}
		i.x += level->ui.raycast_quality;
	}
	if (avg)
		avg = avg / (float)count;
	avg *= level->ui.ssao_intensity;


	if (avg > 1)
		return (1);
	else if (avg < 0)
		return (0);
	return (avg);
}

void	ssao_calculate(t_window *win, t_level *level, int thread_id)
{
	t_ivec2	i;
	float	darkness;

	i.x = 0;
	i.x = thread_id * level->ui.raycast_quality;
	while (i.x < RES_X)
	{
		i.y = 0;
		while (i.y < RES_Y)
		{
			if (win->depth_buffer[i.x + i.y * RES_X] != FLT_MAX)
			{
				darkness = surrounding_diff(i.x, i.y, level, win);
				// if (x == RES_X / 2 && y == RES_Y / 2)
				// 	win->frame_buffer[x + y * RES_X] = 0xff0000ff;
				// else
				if (level->ui.ssao_debug)
					win->frame_buffer[i.x + i.y * RES_X] = crossfade((unsigned)0xffffff, (unsigned)0, darkness * 0xff, 0xff);
				else
					win->frame_buffer[i.x + i.y * RES_X] = crossfade((unsigned)win->frame_buffer[i.x + i.y * RES_X] >> 8, (unsigned)0x000000ff >> 8, darkness * 0xff, 0xff);
			}
			// win->frame_buffer[x + y * RES_X] = darkness;
			// win->depth_buffer[];
			i.y += level->ui.raycast_quality;
		}
		i.x += THREAD_AMOUNT * level->ui.raycast_quality;
	}
}

int	ssao_init(void *data_pointer)
{
	t_rthread	*thread;

	thread = data_pointer;
	ssao_calculate(thread->window, thread->level, thread->id);
	return (0);
}

void	ssao(t_window *window, t_level *level)
{
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		thread_data[THREAD_AMOUNT];
	int				i;

	i = -1;
	while (++i < THREAD_AMOUNT)
	{
		thread_data[i].id = i;
		thread_data[i].level = level;
		thread_data[i].window = window;
		threads[i] = SDL_CreateThread(ssao_init, "ssao",
				(void *)&thread_data[i]);
	}
	i = -1;
	while (++i < THREAD_AMOUNT)
		SDL_WaitThread(threads[i], NULL);
}
