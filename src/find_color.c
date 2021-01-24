/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_color.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/21 20:36:38 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	line_intersection(float res[2], float a[2], float b[2], float c[2], float d[2]) 
{
	// Line AB represented as a1x + b1y = c1
	double a1 = b[1] - a[1];
	double b1 = a[0] - b[0];
	double c1 = a1 * a[0] + b1 * a[1];

	// Line CD represented as a2x + b2y = c2
	double a2 = d[1] - c[1];
	double b2 = c[0] - d[0];
	double c2 = a2 * c[0] + b2 * c[1];

	double determinant = a1*b2 - a2*b1;

	// if (determinant == 0)
	// {
		// The lines are parallel. This is simplified
		// by returning a pair of FLT_MAX
		// return make_pair(FLT_MAX, FLT_MAX);
	// }
	// els
	{
		res[0] = (b2*c1 - b1*c2)/determinant;
		res[1] = (a1*c2 - a2*c1)/determinant;
	}
}

void	rot2d(float res[2], float v[2], float rad)
{
	float sn = sin(rad);
	float cs = sin(rad);

	res[0] = v[0] * cs - v[1] * sn; 
	res[1] = v[0] * sn + v[1] * cs;
}

unsigned	crossfade(unsigned color1, unsigned color2, unsigned fade, unsigned r1)
{
	unsigned g1;
	unsigned b1;
	unsigned r2;
	unsigned g2;
	unsigned b2;

	r1 = ((color1 % (0x1000000)) >> 8 * 2);
	g1 = ((color1 % (0x10000)) >> 8 * 1);
	b1 = (color1 % (0x100));
	r2 = ((color2 % (0x1000000)) >> 8 * 2);
	g2 = ((color2 % (0x10000)) >> 8 * 1);
	b2 = (color2 % (0x100));

	unsigned newr = (r1 * (0xff - fade) + r2 * fade) / 0xff;
	unsigned newg = (g1 * (0xff - fade) + g2 * fade) / 0xff;
	unsigned newb = (b1 * (0xff - fade) + b2 * fade) / 0xff;
	return ((newr << 8 * 3) + (newg << 8 * 2) + (newb << 8 * 1) + 0xff);
}

int		find_color(float u, float v, t_tri t, t_ray r, float dist, t_bmp *img, unsigned fog_color)
{
	int		col;
	float	v0v1[2];
	float	v0v2[2];
	// v1m0[2] = t.verts[1].txtr[0] * img->width - t.verts[0].txtr[0] * img-

	v0v1[0] = (t.verts[1].txtr[0] - t.verts[0].txtr[0]) * v;
	v0v1[1] = (t.verts[1].txtr[1] - t.verts[0].txtr[1]) * v;

	v0v2[0] = (t.verts[2].txtr[0] - t.verts[0].txtr[0]) * u;
	v0v2[1] = (t.verts[2].txtr[1] - t.verts[0].txtr[1]) * u;

	v0v1[0] += t.verts[0].txtr[0];
	v0v1[1] += t.verts[0].txtr[1];
	v0v2[0] += t.verts[0].txtr[0];
	v0v2[1] += t.verts[0].txtr[1];

	v0v1[0] *= img->width;
	v0v2[0] *= img->width;
	v0v1[1] *= img->height;
	v0v2[1] *= img->height;

	int x = v0v2[0];
	int y = img->height - v0v1[1];

	col = fog_color;
	if (dist < 20)
	{
		if (x + (y * img->width) < img->width * img->height &&
			x + (y * img->width) >= 0)
			col = img->image[x + (y * img->width)];

		col = (((int)(u * 255) & 0xff) << 24) +
				(((int)(v * 255) & 0xff) << 16) +
				(((int)((1-u-v) * 255) & 0xff) << 8) +
				0xff;
		float fade = (dist + 1) / 20;
		fade = fade > 1 ? 1 : fade;
		col = crossfade(col >> 8, fog_color >> 8, 0xff * fade, 0);
	}
	return (col);
}
