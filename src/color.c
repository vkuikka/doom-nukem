/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/01 18:37:17 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"


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

int			skybox(t_level l, t_ray r)
{
	int		color;
	float	dist;

	color = 0;
	r.pos.x = 0;
	r.pos.y = 0;
	r.pos.z = 0;
	for (int i = 0; i < l.sky.obj.tri_amount; i++)
	{
		dist = cast_face(l.sky.obj.tris[i] , r, &color, &l.sky.img);
		if (dist > 0 && color)
			return (color);
	}
	return (color);
}

int			fog(int color, float dist, unsigned fog_color, t_level *level)
{
	float	fade;

	if (dist < level->ui->render_distance)
	{
		fade = (dist + 1) / (level->ui->render_distance - 1);
		fade = fade > 1 ? 1 : fade;
		return (crossfade(color >> 8, fog_color >> 8, 0xff * fade, 0));
	}
	return (fog_color);
}

void		blur_pixels(unsigned *color, int gap)
{
	int		res;
	int		x;
	int		y;

	y = gap;
	res = 0;
	while (y < RES_Y - gap)
	{
		x = gap;
		while (x < RES_X - gap)
		{
			res = color[x + (y * RES_X)];
			int col1 = color[x - gap + (y * RES_X)];
			int col2 = color[x + ((y - gap) * RES_X)];
			int col3 = color[x + gap + (y * RES_X)];
			int col4 = color[x + ((y + gap) * RES_X)];
			float fade = 1.0 / 4.0;
			res = crossfade(res >> 8, col1 >> 8, fade * 0xff, 0);
			res = crossfade(res >> 8, col2 >> 8, fade * 0xff, 0);
			res = crossfade(res >> 8, col3 >> 8, fade * 0xff, 0);
			res = crossfade(res >> 8, col4 >> 8, fade * 0xff, 0);
			color[x + (y * RES_X)] = res;
			x += gap;
		}
		y += gap;
	}
}

int			smooth_color(unsigned *pixels, int gap, int x, int y)
{
	int		dx;
	int		dy;
	int		re1 = 0;
	int		re2 = 0;
	int		tmp = 0;

	dx = x - x % gap;
	dy = y - y % gap;
	if (x >= RES_X - gap && y >= RES_Y - gap)
		return(pixels[dx + ((y - y % gap) * RES_X)]);
	if (x >= RES_X - gap)
	{
		re1 = pixels[dx + dy * RES_X];
		re2 = pixels[dx + (dy + gap) * RES_X];
		return(crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0));
	}
	if (y >= RES_Y - gap)
	{
		re1 = pixels[dx + dy * RES_X];
		re2 = pixels[dx + gap + dy * RES_X];
		return(crossfade(re1 >> 8, re2 >> 8, x % gap / (float)gap * 0xff, 0));
	}
	re1 = pixels[dx + dy * RES_X];
	re2 = pixels[dx + (dy + gap) * RES_X];
	tmp = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0);
	re1 = pixels[dx + gap + dy * RES_X];
	re2 = pixels[dx + gap + (dy + gap) * RES_X];
	re1 = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0);
	return(crossfade(tmp >> 8, re1 >> 8, x % gap / (float)gap * 0xff, 0));
}

void		fill_pixels(unsigned *grid, int gap, int blur, int smooth)
{
	int		color;
	int		i;
	int		x;
	int		y;

	y = 0;
	if (blur)
		blur_pixels(grid, gap);
	while (y < RES_Y)
	{
		x = 0;
		color = 0;
		while (x < RES_X)
		{
			if (smooth)
			{
				if (x % gap || y % gap)
					grid[x + (y * RES_X)] = smooth_color(grid, gap, x, y);
			}
			else if (!(x % gap))
			{
				color = grid[x + (y * RES_X)];
				if ((y + 1) % gap && y + 1 < RES_Y)
					grid[x + ((y + 1) * RES_X)] = color;
			}
			else
				grid[x + (y * RES_X)] = color;
			x++;
		}
		y++;
	}
}

int		face_color(float u, float v, t_tri t, t_bmp *img)
{
	int		x;
	int 	y;
	float	w;

	w = 1 - u - v;
	// if (level->ui->show_quads)
	// 	return((((int)(u * 255) & 0xff) << 24) +
	// 			(((int)(v * 255) & 0xff) << 16) +
	//  		(((int)(w * 255) & 0xff) << 8) + 0xff);
	global_seginfo = "face_color\n";
	x =	((t.verts[0].txtr.x * img->width * w +
			t.verts[1].txtr.x * img->width * v +
			t.verts[2].txtr.x * img->width * u) / (float)(u + v + w));
	y =	((t.verts[0].txtr.y * img->height * w +
			t.verts[1].txtr.y * img->height * v +
			t.verts[2].txtr.y * img->height * u) / (float)(u + v + w));
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
