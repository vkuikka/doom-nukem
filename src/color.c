/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/26 02:18:41 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

// int		skybox_color(t_ray r, t_bmp *img)
// {
// 	return (0);
// }

int		face_color(float u, float v, t_tri t, t_bmp *img)
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
