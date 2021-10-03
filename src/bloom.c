/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bloom.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/01 15:59:43 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/04 02:05:21 by vkuikka          ###   ########.fr       */
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

int	init_gradient(float *arr)
{
	t_vec2	i;
	t_vec2	mid;
	t_vec2	diff;
	float	div;
	float	new;

	mid.x = BOX_BLUR_GRADIENT_SIZE / 2;
	mid.y = BOX_BLUR_GRADIENT_SIZE / 2;
	diff.x = 0.5 * BOX_BLUR_GRADIENT_SIZE;
	diff.y = 0;
	vec2_sub(&diff, mid, diff);
	div = vec2_length(diff);
	i.x = 0;
	while (i.x < BOX_BLUR_GRADIENT_SIZE)
	{
		i.y = 0;
		while (i.y < BOX_BLUR_GRADIENT_SIZE)
		{
			vec2_sub(&diff, mid, i);
			new = 1 - (vec2_length(diff) / div);
			// new = cos(new * M_PI + M_PI);
			if (new < 0)
				new = 0;
			arr[(int)i.x + (int)i.y * BOX_BLUR_GRADIENT_SIZE] = new;
			i.y++;
		}
		i.x++;
	}
	for (int i = 0; i < BOX_BLUR_GRADIENT_SIZE; i++)
	{
		for (int j = 0; j < BOX_BLUR_GRADIENT_SIZE; j++)
			printf("%.2f ", arr[i + j * BOX_BLUR_GRADIENT_SIZE]);
		printf("\n");
	}
	return (0);
}

void	box_avg(t_ivec2 p, t_color *pixels, int radius, t_level *level)
{
	static float	*gradient_table = NULL;
	t_ivec2			i;
	t_ivec2			bound;
	unsigned int	tmp;
	t_ivec2			start;
	t_color			*buff;

	float			amount = 0;
	unsigned int	t = p.x + p.y * RES_X;

	if (!gradient_table)
	{
		gradient_table = (float *)malloc(sizeof(float)
						* BOX_BLUR_GRADIENT_SIZE * BOX_BLUR_GRADIENT_SIZE);
		if (!gradient_table)
			return;
		if (init_gradient(gradient_table))
			return;
	}
	buff = get_buff(NULL);
	bound.x = p.x + radius;
	bound.y = p.y + radius;
	start.y = p.y - radius;
	start.x = p.x - radius;
	if (start.y < 0)
		start.y = 0;
	if (start.x < 0)
		start.x = 0;
	if (start.y % level->ui.raycast_quality)
		start.y += level->ui.raycast_quality - start.y % level->ui.raycast_quality;
	if (start.x % level->ui.raycast_quality)
		start.x += level->ui.raycast_quality - start.x % level->ui.raycast_quality;

	i.x = start.x;
	while (i.x < bound.x)
	{
		i.y = start.y;
		while (i.y < bound.y)
		{
			tmp = i.x + i.y * RES_X;
			if (tmp > 0 && tmp < RES_X * RES_Y)
			{
				int x = ((float)(i.x - start.x) / (radius * 2)) * BOX_BLUR_GRADIENT_SIZE;
				int y = ((float)(i.y - start.y) / (radius * 2)) * BOX_BLUR_GRADIENT_SIZE;
				float grad = gradient_table[x + y * BOX_BLUR_GRADIENT_SIZE];

				buff[t].r += pixels[tmp].r * level->ui.bloom_intensity * grad;
				buff[t].g += pixels[tmp].g * level->ui.bloom_intensity * grad;
				buff[t].b += pixels[tmp].b * level->ui.bloom_intensity * grad;
				amount += 1 + grad;
			}
			i.y += level->ui.raycast_quality;
		}
		i.x += level->ui.raycast_quality;
	}
	if (level->ui.bloom_debug)
		return;
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

int	bloom_init(void *data_pointer)
{
	t_rthread	*thread;

	thread = data_pointer;
	box_blur(thread->window->brightness_buffer, thread->level->ui.bloom_radius, thread->level, thread->id);
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
