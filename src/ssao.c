/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ssao.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 13:56:14 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

float	pixel_dot_product(t_ivec2 i, t_ivec2 mid, t_window *win)
{
	t_vec3	v1;
	t_vec3	v2;

	v2 = win->pixel_pos_buffer[i.x + i.y * RES_X];
	v1 = win->pixel_pos_buffer[mid.x + mid.y * RES_X];
	vec_sub(&v2, v2, v1);
	vec_normalize(&v2);
	vec_normalize(&v1);
	return (-vec_dot(v2, v1));
}

void	ssao_kernel_iter(t_ssao *ssao, t_window *win, t_ivec2 i, float br)
{
	float	dist;
	float	diff;
	float	val;

	dist = win->depth_buffer[i.x + i.y * RES_X];
	diff = win->depth_buffer[ssao->kernel_center.x
		+ ssao->kernel_center.y * RES_X] - dist;
	val = pixel_dot_product(i, ssao->kernel_center, win);
	if (dist != FLT_MAX && !isnan(val))
	{
		val /= br;
		if (diff > 1)
		{
			val /= diff;
			ssao->count += diff - 1;
		}
		ssao->total += radial_gradient(i, ssao->lower_bound, ssao->radius * 2)
			* val;
	}
	else if (dist == FLT_MAX)
		ssao->total -= radial_gradient(i, ssao->lower_bound, ssao->radius * 2);
	ssao->count++;
}

void	ssao_bounds(t_ssao *ssao)
{
	ssao->lower_bound.x = ssao->kernel_center.x - ssao->radius;
	ssao->lower_bound.y = ssao->kernel_center.y - ssao->radius;
	ssao->upper_bound.x = ssao->kernel_center.x + ssao->radius;
	ssao->upper_bound.y = ssao->kernel_center.y + ssao->radius;
	if (ssao->lower_bound.y < 0)
		ssao->lower_bound.y = 0;
	if (ssao->lower_bound.x < 0)
		ssao->lower_bound.x = 0;
	if (ssao->upper_bound.y > RES_Y)
		ssao->upper_bound.y = RES_Y;
	if (ssao->upper_bound.x > RES_X)
		ssao->upper_bound.x = RES_X;
}

float	ssao_avg(t_ssao ssao, t_level *level)
{
	float	avg;

	avg = 0;
	if (ssao.count <= 0)
		ssao.total = 0;
	if (ssao.total)
		avg = ssao.total / (float)ssao.count;
	avg *= level->ui.ssao_intensity;
	if (avg > 1)
		return (1);
	else if (avg < 0)
		return (0);
	return (avg);
}

float	surrounding_diff(t_ssao ssao, t_level *level, t_window *win)
{
	t_ivec2	i;
	t_color	light;
	float	brightness;

	ssao_bounds(&ssao);
	ssao.count = 0;
	ssao.total = 0;
	i.x = ssao.lower_bound.x;
	while (i.x < ssao.upper_bound.x)
	{
		i.y = ssao.lower_bound.y;
		while (i.y < ssao.upper_bound.y)
		{
			light = win->brightness_buffer[i.x + i.y * RES_X];
			brightness = (light.r + light.g + light.b)
				* level->ui.ssao_light_bias;
			if (brightness > 1)
				ssao_kernel_iter(&ssao, win, i, brightness);
			else
				ssao_kernel_iter(&ssao, win, i, 1);
			i.y += level->ui.raycast_quality;
		}
		i.x += level->ui.raycast_quality;
	}
	return (ssao_avg(ssao, level));
}

void	ssao_color(t_window *win, t_level *level, float amount, t_ivec2 coords)
{
	unsigned int	pixel;

	pixel = coords.x + coords.y * RES_X;
	if (level->ui.ssao_debug)
		win->frame_buffer[pixel] = crossfade(0xffffff, 0, amount * 0xff, 0xff);
	else
		win->frame_buffer[pixel] = crossfade(win->frame_buffer[pixel] >> 8, 0,
				amount * 0xff, 0xff);
}

void	ssao_calculate(t_window *win, t_level *level, int thread_id)
{
	t_ivec2	i;
	t_ssao	ssao;
	float	darkness;

	ssao.radius = level->ui.ssao_radius;
	ssao.radius -= ssao.radius % level->ui.raycast_quality;
	i.x = thread_id * level->ui.raycast_quality;
	while (i.x < RES_X)
	{
		i.y = 0;
		while (i.y < RES_Y)
		{
			if (win->depth_buffer[i.x + i.y * RES_X] != FLT_MAX)
			{
				ssao.kernel_center = i;
				darkness = surrounding_diff(ssao, level, win);
				ssao_color(win, level, darkness, i);
			}
			i.y += level->ui.raycast_quality;
		}
		i.x += THREAD_AMOUNT * level->ui.raycast_quality;
	}
}

int	ssao_thread(void *data_pointer)
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
		threads[i] = SDL_CreateThread(ssao_thread, "ssao",
				(void *)&thread_data[i]);
	}
	i = -1;
	while (++i < THREAD_AMOUNT)
		SDL_WaitThread(threads[i], NULL);
}
