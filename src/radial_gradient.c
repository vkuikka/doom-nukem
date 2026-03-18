/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   radial_gradient_0.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

float	gradient_function(t_vec2 diff, t_vec2 mid, float div, t_vec2 i)
{
	float	new;

	vec2_sub(&diff, mid, i);
	new = 1 - (vec2_length(diff) / div);
	new = asin(new * 2 - 1) / M_PI + 0.5;
	if (new < 0 || isnan(new))
		new = 0;
	return (new);
}

int	init_gradient(float *arr)
{
	t_vec2	i;
	t_vec2	mid;
	t_vec2	diff;
	float	div;

	mid.x = RADIAL_GRADIENT_RESOLUTION / 2;
	mid.y = RADIAL_GRADIENT_RESOLUTION / 2;
	diff.x = RADIAL_GRADIENT_RESOLUTION / 2;
	diff.y = 0;
	vec2_sub(&diff, mid, diff);
	div = vec2_length(diff);
	i.x = -1;
	while (++i.x < RADIAL_GRADIENT_RESOLUTION)
	{
		i.y = -1;
		while (++i.y < RADIAL_GRADIENT_RESOLUTION)
			arr[(int)i.x + (int)i.y * RADIAL_GRADIENT_RESOLUTION]
				= gradient_function(diff, mid, div, i);
	}
	return (0);
}

float	radial_gradient(t_ivec2 pixel, t_ivec2 lower_bound, float diameter)
{
	static float	*gradient_table = NULL;
	int				x;
	int				y;

	if (!gradient_table)
	{
		gradient_table = (float *)malloc(sizeof(float)
				* RADIAL_GRADIENT_RESOLUTION * RADIAL_GRADIENT_RESOLUTION);
		if (!gradient_table)
			return (0);
		init_gradient(gradient_table);
	}
	x = ((pixel.x - lower_bound.x) / diameter) * RADIAL_GRADIENT_RESOLUTION;
	y = ((pixel.y - lower_bound.y) / diameter) * RADIAL_GRADIENT_RESOLUTION;
	return (gradient_table[x + y * RADIAL_GRADIENT_RESOLUTION]);
}
