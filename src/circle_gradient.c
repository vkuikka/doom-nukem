/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   circle_gradient.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/20 22:07:43 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/20 22:33:44 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

/*
	gradient array looks something like this:

	.0 .0 .1 .1 .0 .0
	.0 .1 .5 .5 .1 .0
	.1 .5 1. 1. .5 .1
	.1 .5 1. 1. .5 .1
	.0 .1 .5 .5 .1 .0
	.0 .0 .1 .1 .0 .0

	from edge to center values increase following arcsine
*/

static float	gradient_function(t_vec2 diff, t_vec2 mid, float div, t_vec2 i)
{
	float	new;

	vec2_sub(&diff, mid, i);
	new = 1 - (vec2_length(diff) / div);
	new = asin(new * 2 - 1) / M_PI + 0.5;
	if (new < 0 || isnan(new))
		new = 0;
	return (new);
}

static int	init_gradient(float *arr)
{
	t_vec2	i;
	t_vec2	mid;
	t_vec2	diff;
	float	div;

	mid.x = CIRCLE_GRADIENT_RESOLUTION / 2;
	mid.y = CIRCLE_GRADIENT_RESOLUTION / 2;
	diff.x = CIRCLE_GRADIENT_RESOLUTION / 2;
	diff.y = 0;
	vec2_sub(&diff, mid, diff);
	div = vec2_length(diff);
	i.x = -1;
	while (++i.x < CIRCLE_GRADIENT_RESOLUTION)
	{
		i.y = -1;
		while (++i.y < CIRCLE_GRADIENT_RESOLUTION)
			arr[(int)i.x + (int)i.y * CIRCLE_GRADIENT_RESOLUTION]
				= gradient_function(diff, mid, div, i);
	}
	return (0);
}

float	circle_gradient(t_ivec2 pixel, t_ivec2 lower_bound, float diameter)
{
	static float	*gradient_table = NULL;
	int				x;
	int				y;

	if (!gradient_table)
	{
		gradient_table = (float *)malloc(sizeof(float)
				* CIRCLE_GRADIENT_RESOLUTION * CIRCLE_GRADIENT_RESOLUTION);
		if (!gradient_table)
			return (0);
		init_gradient(gradient_table);
	}
	x = ((pixel.x - lower_bound.x) / diameter) * CIRCLE_GRADIENT_RESOLUTION;
	y = ((pixel.y - lower_bound.y) / diameter) * CIRCLE_GRADIENT_RESOLUTION;
	return (gradient_table[x + y * CIRCLE_GRADIENT_RESOLUTION]);
}
