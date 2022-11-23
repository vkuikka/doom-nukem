/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   distortion_0.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

unsigned int	noise(unsigned int color, float probability, float amount)
{
	unsigned char	*rgb;

	rgb = (unsigned char *)&color;
	if (rand() < RAND_MAX * probability)
	{
		if ((int)(rgb[0] * amount))
		{
			rgb[3] += rand() % (int)(rgb[0] * amount);
			rgb[2] += rand() % (int)(rgb[0] * amount);
			rgb[1] += rand() % (int)(rgb[0] * amount);
			rgb[0] += rand() % (int)(rgb[0] * amount);
		}
	}
	return ((unsigned int)*(int *)rgb);
}

unsigned int	black_and_white(unsigned int color, float amount)
{
	unsigned char	*rgb;
	unsigned int	res;

	rgb = (unsigned char *)&color;
	res = 0;
	res += rgb[3];
	res += rgb[2];
	res += rgb[1];
	res /= 3;
	rgb[3] = res * amount + rgb[3] * (1 - amount);
	rgb[2] = res * amount + rgb[2] * (1 - amount);
	rgb[1] = res * amount + rgb[1] * (1 - amount);
	return ((unsigned int)*(int *)rgb);
}

unsigned int	chroma_px(t_bmp *img, int x, int y, int x_amount)
{
	unsigned int	res_color;
	unsigned char	*rgb_l;
	unsigned char	*rgb;
	unsigned char	*rgb_r;
	unsigned char	*res;

	rgb = (unsigned char *)&img->image[x + y * img->width];
	rgb_l = (unsigned char *)&img->image[x + y * img->width];
	rgb_r = (unsigned char *)&img->image[x + y * img->width];
	if (x_amount < 0)
		x_amount *= -1;
	if (x - x_amount > 0)
		rgb_l = (unsigned char *)&img->image[(x - x_amount) + y * img->width];
	if (x + x_amount < img->width)
		rgb_r = (unsigned char *)&img->image[(x + x_amount) + y * img->width];
	res = (unsigned char *)&res_color;
	res[3] = rgb_l[3];
	res[2] = rgb[2];
	res[1] = rgb_r[1];
	res[0] = rgb[0];
	return (res_color);
}

void	chromatic_abberation(unsigned int *pixels, unsigned int *buf,
														int amount)
{
	t_bmp	tmp;
	int		x;
	int		y;

	tmp.width = RES_X;
	tmp.height = RES_Y;
	tmp.image = (int *)buf;
	ft_memcpy(buf, pixels, sizeof(int) * RES_X * RES_Y);
	y = 0;
	while (y < RES_Y)
	{
		x = 0;
		while (x < RES_X)
		{
			pixels[x + y * RES_X] = chroma_px(&tmp, x, y, amount);
			x++;
		}
		y++;
	}
}

void	fake_analog_signal(t_bmp *img, unsigned int *pixels, float amount)
{
	static int		disturbance_y = 0;
	int				x_offset;
	unsigned int	color;
	t_ivec2			i;

	disturbance_y++;
	disturbance_y = disturbance_y % img->height;
	i.y = -1;
	while (++i.y < img->height)
	{
		x_offset = (rand() % (int)(3 + 12 * amount)) - 6;
		if (i.y > disturbance_y - 5 && i.y < disturbance_y + 5)
			x_offset = (rand() % 15) - 6;
		i.x = -1;
		while (++i.x < img->width)
		{
			color = chroma_px(img, i.x, i.y, x_offset + 2);
			color = noise(color, amount / 8, .2);
			if (amount > .9)
				color = black_and_white(color, (amount - .9) / .1 * .7);
			pixels[(RES_X / 2 - img->width / 2) + x_offset + i.x
				+ i.y * RES_X] = color;
		}
	}
}
