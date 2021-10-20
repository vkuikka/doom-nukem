/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ssao.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 14:38:16 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/20 22:14:06 by vkuikka          ###   ########.fr       */
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

	i.x = x - radius;
	count = 0;
	avg = 0;
	while (i.x < x + radius)
	{
		i.y = y - radius;
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
					avg += val;
					count++;
				}
			}
			i.y += level->ui.raycast_quality;
		}
		i.x += level->ui.raycast_quality;
	}
	float res = avg / (float)count;

	if (x == RES_X / 2 && y == RES_Y / 2)
		printf("%f\n", res);

	if (res > 1)
		return (1);
	else if (res < 0)
		return (0);
	return (res);
}

void		ssao(t_window *win, t_level *level)
{
	float		darkness;

	for (int x = 0; x < RES_X; x += level->ui.raycast_quality)
	{
		for (int y = 0; y < RES_Y; y += level->ui.raycast_quality)
		{
			if (win->depth_buffer[x + y * RES_X] != FLT_MAX)
			{
				darkness = surrounding_diff(x, y, level, win);

				if (x == RES_X / 2 && y == RES_Y / 2)
					win->frame_buffer[x + y * RES_X] = 0xff0000ff;
				else
					// win->frame_buffer[x + y * RES_X] = crossfade((unsigned)win->frame_buffer[x + y * RES_X] >> 8, (unsigned)0x000000ff >> 8, darkness * 0xff, 0xff);
					win->frame_buffer[x + y * RES_X] = crossfade((unsigned)0xffffff, (unsigned)0, darkness * 0xff, 0xff);
			}
			// win->frame_buffer[x + y * RES_X] = darkness;
			// win->depth_buffer[];
		}
	}

}
