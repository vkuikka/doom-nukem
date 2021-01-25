/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_color.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/26 01:23:23 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	ft_draw_line(int line[4], unsigned int *frame, int c)
{
	float	x_increment;
	float	y_increment;
	float	xy[2];
	int		steps;
	int		i;

	i = 0;
	xy[0] = line[0];
	xy[1] = line[1];
	steps = abs(line[2] - line[0]) > abs(line[3] - line[1]) ?
			abs(line[2] - line[0]) : abs(line[3] - line[1]);
	if (!steps)
		return ;
	x_increment = (float)(line[2] - line[0]) / (float)steps;
	y_increment = (float)(line[3] - line[1]) / (float)steps;
	while (i <= (steps))
	{
		int p = xy[0] + (int)RES_X * (int)xy[1];
		if (p < (int)RES_X * (int)RES_Y && p >= 0)
			frame[p] = c;

		xy[0] += x_increment;
		xy[1] += y_increment;
		i++;
	}
}

void	line_intersection(float res[2], float a[2], float b[2], float c[2], float d[2])
{
	// Line AB represented as a1x + b1y = c1
	float a1 = b[1] - a[1];
	float b1 = a[0] - b[0];
	float c1 = a1 * a[0] + b1 * a[1];

	// Line CD represented as a2x + b2y = c2
	float a2 = d[1] - c[1];
	float b2 = c[0] - d[0];
	float c2 = a2 * c[0] + b2 * c[1];

	float determinant = a1*b2 - a2*b1;

	// if (determinant == 0)
		// The lines are parallel. This is simplified
	// else
	{
		res[0] = (b2*c1 - b1*c2)/determinant;
		res[1] = (a1*c2 - a2*c1)/determinant;
	}
}

int		find_color(float u, float v, t_tri t, t_bmp *img)
{
	int		x;
	int 	y;
	float	w;

	w = 1 - u - v;
	x =	((t.verts[0].txtr[0] * img->width * w +
			t.verts[1].txtr[0] * img->width * v +
			t.verts[2].txtr[0] * img->width * u) / (float)(u + v + w));
	y =	((t.verts[0].txtr[1] * img->height * w +
			t.verts[1].txtr[1] * img->height * v +
			t.verts[2].txtr[1] * img->height * u) / (float)(u + v + w));
	if (y > img->height)
		y = y % img->height;
	else if (y < 0)
		y = -y % img->height;
	y = img->height - y;
	if (x > img->width)
		x = x % img->width;
	else if (x < 0)
		x = -x % img->width;
	return (img->image[x + (y * img->width)]);
}

	// col = (((int)(0 * 255) & 0xff) << 24) +
	// 		(((int)(v * 255) & 0xff) << 16) +
	// 		(((int)(0 * 255) & 0xff) << 8) + 0xff;

	//	following is code for showing uv map usage on screen

	// 	line[0] = t.verts[0].txtr[0] * RES_Y;
	// 	line[1] = t.verts[0].txtr[1] * RES_Y;
	// 	line[2] = t.verts[1].txtr[0] * RES_Y;
	// 	line[3] = t.verts[1].txtr[1] * RES_Y;
	// 	ft_draw_line(line, frame, 0xffffffff);
	// 	line[0] = t.verts[0].txtr[0] * RES_Y;
	// 	line[1] = t.verts[0].txtr[1] * RES_Y;
	// 	line[2] = t.verts[2].txtr[0] * RES_Y;
	// 	line[3] = t.verts[2].txtr[1] * RES_Y;
	// 	ft_draw_line(line, frame, 0xffffffff);
	// 	line[0] = t.verts[1].txtr[0] * RES_Y;
	// 	line[1] = t.verts[1].txtr[1] * RES_Y;
	// 	line[2] = t.verts[2].txtr[0] * RES_Y;
	// 	line[3] = t.verts[2].txtr[1] * RES_Y;
	// 	ft_draw_line(line, frame, 0xffffffff);

	// 	line[0] = t.verts[0].txtr[0] * RES_Y;
	// 	line[1] = t.verts[0].txtr[1] * RES_Y;
	// 	line[2] = (int)(res[0] / img->width * RES_Y);
	// 	line[3] = (int)(res[1] / img->height * RES_Y);
	// 	ft_draw_line(line, frame, 0xffffffff);


	// 	line[0] = (int)(line1start[0] / img->width * RES_Y);
	// 	line[1] = (int)(line1start[1] / img->height * RES_Y);
	// 	line[2] = (int)(line1end[0] / img->height * RES_Y);
	// 	line[3] = (int)(line1end[1] / img->height * RES_Y);
	// 	ft_draw_line(line, frame, 0xffffffff);
	// 	line[0] = (int)(line2start[0] / img->width * RES_Y);
	// 	line[1] = (int)(line2start[1] / img->height * RES_Y);
	// 	line[2] = (int)(line2end[0] / img->width * RES_Y);
	// 	line[3] = (int)(line2end[1] / img->height * RES_Y);
	// 	ft_draw_line(line, frame, 0xffffffff);
