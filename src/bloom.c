/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bloom.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/01 15:59:43 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/03 19:03:31 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

t_color *get_buff(t_color *set)
{
	static t_color	*buff = NULL;

	if (set)
		buff = set;
	return (buff);
}

void	box_avg(t_ivec2 p, t_color *pixels, int radius, t_level *level)
{
	t_ivec2			i;
	t_ivec2			bound;
	unsigned int	tmp;
	int				start_y;
	t_color			*buff;

	float	amount = 0;
	unsigned int	t;
	t = p.x + p.y * RES_X;

	buff = get_buff(NULL);
	bound.x = p.x + radius;
	bound.y = p.y + radius;
	start_y = p.y - radius;
	if (start_y < 0)
		start_y = 0;
	i.x = p.x - radius;
	if (i.x < 0)
		i.x = 0;
	if (i.x % level->ui.raycast_quality)
		i.x += level->ui.raycast_quality - i.x % level->ui.raycast_quality;
	if (start_y % level->ui.raycast_quality)
		start_y += level->ui.raycast_quality - start_y % level->ui.raycast_quality;
	while (i.x < bound.x)
	{
		i.y = start_y;
		while (i.y < bound.y)
		{
			tmp = i.x + i.y * RES_X;
			if (tmp > 0 && tmp < RES_X * RES_Y)
			{
				if (pixels[tmp].r || pixels[tmp].g || pixels[tmp].b)	//fix and remove
				{
					buff[t].r += pixels[tmp].r * level->ui.bloom_intensity;
					buff[t].g += pixels[tmp].g * level->ui.bloom_intensity;
					buff[t].b += pixels[tmp].b * level->ui.bloom_intensity;
					amount++;
				}
			}
			i.y += level->ui.raycast_quality;
		}
		i.x += level->ui.raycast_quality;
	}
	buff[t].r /= (int)amount;
	buff[t].g /= (int)amount;
	buff[t].b /= (int)amount;
}

void	box_blur(t_color *pixels, int radius, t_level *level, int thread)
{
	t_ivec2	p;

	p.x = thread;
	while (p.x < RES_X)
	{
		p.y = 0;
		while (p.y < RES_Y)
		{
			if (!(p.y % level->ui.raycast_quality)
				&& !(p.x % level->ui.raycast_quality))
				box_avg(p, pixels, radius, level);
			p.y++;
		}
		p.x += THREAD_AMOUNT;
	}
}

unsigned int	color_to_int(t_color color)
{
	unsigned int	res;
	unsigned char	*rgb;

	rgb = (unsigned char *)&res;
	color.r = clamp(color.r, 0, 1);
	color.g = clamp(color.g, 0, 1);
	color.b = clamp(color.b, 0, 1);
	rgb[3] = color.r * 0xff;
	rgb[2] = color.g * 0xff;
	rgb[1] = color.b * 0xff;
	rgb[0] = 0xff;
	return (res);
}

t_color	int_to_color(unsigned int color)
{
	unsigned char	*rgb;
	t_color			res;

	rgb = (unsigned char *)&color;
	res.r = rgb[3] / (float)0xff;
	res.g = rgb[2] / (float)0xff;
	res.b = rgb[1] / (float)0xff;
	return (res);
}

void	bloom_apply(t_window *window, t_color *buff)
{
	t_ivec2			p;
	t_color			tmp;
	unsigned int	pixel_color;

	p.x = 0;
	while (p.x < RES_X)
	{
		p.y = 0;
		while (p.y < RES_Y)
		{
			pixel_color = window->frame_buffer[p.x + p.y * RES_X];
			tmp = int_to_color(pixel_color);
			tmp.r += buff[p.x + p.y * RES_X].r;
			tmp.g += buff[p.x + p.y * RES_X].g;
			tmp.b += buff[p.x + p.y * RES_X].b;
			tmp.r = clamp(tmp.r, 0, 1);
			tmp.g = clamp(tmp.g, 0, 1);
			tmp.b = clamp(tmp.b, 0, 1);
			window->frame_buffer[p.x + p.y * RES_X] = color_to_int(tmp);
			p.y++;
		}
		p.x++;
	}
}

void	bloom_calculate(t_level *level, t_window *window, int thread)
{
	int i = 0;

	while (i < level->ui.bloom_iterations)
	{
		box_blur(window->brightness_buffer, level->ui.bloom_radius, level, thread);
		i++;
	}
}

int	bloom_init(void *data_pointer)
{
	t_rthread	*thread;

	thread = data_pointer;
	bloom_calculate(thread->level, thread->window, thread->id);
	return (0);
}

void	bloom(t_level *level, t_window *window)
{
	SDL_Thread	*threads[THREAD_AMOUNT];
	t_rthread	thread_data[THREAD_AMOUNT];
	t_color		*buff;
	int			i;

	buff = (t_color *)malloc(sizeof(t_color) * RES_X * RES_Y);
	if (!buff)
		return;
	ft_memset(buff, 0, sizeof(t_color) * RES_X * RES_Y);
	get_buff(buff);
	i = -1;
	while (++i < THREAD_AMOUNT)
	{
		thread_data[i].id = i;
		thread_data[i].level = level;
		thread_data[i].window = window;
		threads[i] = SDL_CreateThread(bloom_init, "asd",
				(void *)&thread_data[i]);
	}
	int asd;
	i = -1;
	while (++i < THREAD_AMOUNT)
		SDL_WaitThread(threads[i], &asd);
	bloom_apply(window, buff);
	free(buff);
}
