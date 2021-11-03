/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bloom.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/01 15:59:43 by vkuikka           #+#    #+#             */
/*   Updated: 2021/11/03 18:10:00 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static t_color	*get_buff(t_color *set)
{
	static t_color	*buff = NULL;

	if (set)
		buff = set;
	return (buff);
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

void	color_cpy(t_color *c1, t_color *c2)
{
	c1->r = c2->r;
	c1->g = c2->g;
	c1->b = c2->b;
}

void	color_add(t_color *c1, t_color *c2)
{
	c1->r += c2->r;
	c1->g += c2->g;
	c1->b += c2->b;
}

void	bloom_apply(t_window *window, t_color *buff, t_level *level)
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
			if (level->ui.bloom_debug)
				color_cpy(&tmp, &buff[p.x + p.y * RES_X]);
			else
				color_add(&tmp, &buff[p.x + p.y * RES_X]);
			tmp.r = clamp(tmp.r, 0, 1);
			tmp.g = clamp(tmp.g, 0, 1);
			tmp.b = clamp(tmp.b, 0, 1);
			window->frame_buffer[p.x + p.y * RES_X] = color_to_int(tmp);
			p.y += level->ui.raycast_quality;
		}
		p.x += level->ui.raycast_quality;
	}
}

int	bloom_init(void *data_pointer)
{
	t_rthread	*thread;
	t_blur		blur;

	thread = data_pointer;
	blur.buff = get_buff(NULL);
	blur.pixels = thread->window->brightness_buffer;
	blur.intensity = thread->level->ui.bloom_intensity;
	blur.quality = thread->level->ui.raycast_quality;
	blur.radius = thread->level->ui.bloom_radius;
	blur.size.x = RES_X;
	blur.size.y = RES_Y;
	box_blur(blur, thread->id);
	return (0);
}

static int	buffer_alloc(t_color **buff)
{
	if (!*buff)
	{
		*buff = (t_color *)malloc(sizeof(t_color) * RES_X * RES_Y);
		if (!*buff)
			return (0);
	}
	return (1);
}

void	bloom(t_level *level, t_window *window)
{
	static t_color	*buff = NULL;
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		thread_data[THREAD_AMOUNT];
	int				i;

	if (!buff && !buffer_alloc(&buff))
		return ;
	memset(buff, 0, sizeof(t_color) * RES_X * RES_Y);
	get_buff(buff);
	i = -1;
	while (++i < THREAD_AMOUNT)
	{
		thread_data[i].id = i;
		thread_data[i].level = level;
		thread_data[i].window = window;
		threads[i] = SDL_CreateThread(bloom_init, "bloom",
				(void *)&thread_data[i]);
	}
	i = -1;
	while (++i < THREAD_AMOUNT)
		SDL_WaitThread(threads[i], NULL);
	bloom_apply(window, buff, level);
}
