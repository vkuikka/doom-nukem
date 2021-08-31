/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color_hsl.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 09:13:43 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/31 14:37:14 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

/*
b (lightness) input range -1, 1
*/
unsigned int	set_lightness(unsigned int color, float b)
{
	unsigned char	*rgb;
	int				i;


	rgb = (unsigned char *)&color;
	i = 1;
	while (i < 4)
	{
		if (b > 0)
			rgb[i] = 0xff * lerp(rgb[i] / (float)0xff, 1, b);
		else
			rgb[i] *= b + 1;
		i++;
	}
	return (color);
}

unsigned int	set_saturation(unsigned int color, float s)
{
	unsigned char	*rgb;
	unsigned int	avg;
	int				i;

	rgb = (unsigned char *)&color;
	avg = (rgb[1] + rgb[2] + rgb[3]) / 3;
	avg *= 1.0 - s;
	i = 1;
	while (i < 4)
	{
		rgb[i] = rgb[i] * s + avg;
		i++;
	}
	return (color);
}

void	hsl_update_color(t_color_hsl *c)
{
	unsigned char	*rgb;
	float			amount;

	rgb = (unsigned char *)&c->color;
	amount = ((sin((c->hue * M_PI * 2) + (M_PI * 2 * ((1.0 / 3) * 1))) + 1) / 2);
	amount *= 2;
	amount = clamp(amount, .5, 1.5) - .5;
	rgb[3] = 0xff * amount;
	amount = ((sin((c->hue * M_PI * 2) + (M_PI * 2 * ((1.0 / 3) * 2))) + 1) / 2);
	amount *= 2;
	amount = clamp(amount, .5, 1.5) - .5;
	rgb[2] = 0xff * amount;
	amount = ((sin((c->hue * M_PI * 2) + (M_PI * 2 * ((1.0 / 3) * 3))) + 1) / 2);
	amount *= 2;
	amount = clamp(amount, .5, 1.5) - .5;
	rgb[1] = 0xff * amount;
	rgb[0] = 0xff;
	c->rgb_hue = c->color;
	c->color = set_lightness(c->color, c->lightness);
	c->color = set_saturation(c->color, c->saturation);
	c->r = rgb[3] / (float)0xff;
	c->g = rgb[2] / (float)0xff;
	c->b = rgb[1] / (float)0xff;
}
