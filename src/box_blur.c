/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   box_blur_0.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	make_bounds(t_blur *blur, t_ivec2 p)
{
	t_ivec2	*upper;
	t_ivec2	*lower;

	upper = &blur->upper_bound;
	lower = &blur->lower_bound;
	upper->x = p.x + blur->radius;
	upper->y = p.y + blur->radius;
	if (upper->y > blur->size.y)
		upper->y = blur->size.y;
	if (upper->x > blur->size.x)
		upper->x = blur->size.x;
	lower->y = p.y - blur->radius;
	lower->x = p.x - blur->radius;
	if (lower->y < 0)
		lower->y = 0;
	if (lower->x < 0)
		lower->x = 0;
	if (lower->y % blur->quality)
		lower->y += blur->quality
			- lower->y % blur->quality;
	if (lower->x % blur->quality)
		lower->x += blur->quality
			- lower->x % blur->quality;
}

float	add_to_buffer(t_blur b, t_ivec2 i, unsigned int coord)
{
	float			grad;
	unsigned int	tmp;

	tmp = i.x + i.y * b.size.x;
	if (b.skip_zeroes
		&& (!b.pixels[tmp].r || !b.pixels[tmp].g || !b.pixels[tmp].b))
		return (0);
	grad = radial_gradient(i, b.lower_bound, b.radius * 2);
	b.buff[coord].r += b.pixels[tmp].r * b.intensity * grad;
	b.buff[coord].g += b.pixels[tmp].g * b.intensity * grad;
	b.buff[coord].b += b.pixels[tmp].b * b.intensity * grad;
	return (grad);
}

void	box_avg(t_ivec2 p, t_blur blur)
{
	t_ivec2			i;
	unsigned int	coord;
	float			amount;

	amount = 0;
	coord = p.x + p.y * blur.size.x;
	make_bounds(&blur, p);
	i.x = blur.lower_bound.x;
	while (i.x < blur.upper_bound.x)
	{
		i.y = blur.lower_bound.y;
		while (i.y < blur.upper_bound.y)
		{
			amount += add_to_buffer(blur, i, coord);
			i.y += blur.quality;
		}
		i.x += blur.quality;
	}
	blur.buff[coord].r /= amount;
	blur.buff[coord].g /= amount;
	blur.buff[coord].b /= amount;
}

void	box_blur(t_blur blur, int thread)
{
	t_ivec2	i;
	int		add_x;

	i.x = thread * blur.quality;
	add_x = THREAD_AMOUNT * blur.quality;
	if (thread == -1)
	{
		i.x = 0;
		add_x = blur.quality;
	}
	while (i.x < blur.size.x)
	{
		i.y = 0;
		while (i.y < blur.size.y)
		{
			if (!blur.skip_zeroes
				|| blur.pixels[i.x + i.y * blur.size.x].r
				|| blur.pixels[i.x + i.y * blur.size.x].g
				|| blur.pixels[i.x + i.y * blur.size.x].b)
				box_avg(i, blur);
			i.y += blur.quality;
		}
		i.x += add_x;
	}
}
