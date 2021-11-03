/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   box_blur.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/03 16:47:47 by vkuikka           #+#    #+#             */
/*   Updated: 2021/11/03 20:06:54 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	make_bounds(t_blur *blur, t_ivec2 p)
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

static float	add_to_buffer(t_blur b, t_ivec2 i, unsigned int coord)
{
	float			grad;
	unsigned int	tmp;

	tmp = i.x + i.y * b.size.x;
	if (!b.pixels[tmp].r || !b.pixels[tmp].g || !b.pixels[tmp].b)
		return (0);
	grad = radial_gradient(i, b.lower_bound, b.radius * 2);
	b.buff[coord].r += b.pixels[tmp].r * b.intensity * grad;
	b.buff[coord].g += b.pixels[tmp].g * b.intensity * grad;
	b.buff[coord].b += b.pixels[tmp].b * b.intensity * grad;
	return (grad);
}

static void	box_avg(t_ivec2 p, t_blur blur)
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

	if (thread == -1)
	{
		add_x = blur.quality;
		i.x = 0;
	}
	else
	{
		add_x = THREAD_AMOUNT * blur.quality;
		i.x = thread * blur.quality;
	}
	while (i.x < blur.size.x)
	{
		i.y = 0;
		while (i.y < blur.size.y)
		{
			box_avg(i, blur);
			i.y += blur.quality;
		}
		i.x += add_x;
	}
}
