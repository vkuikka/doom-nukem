/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/07 12:40:357 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

unsigned		crossfade(unsigned color1, unsigned color2, unsigned fade)
{
	unsigned char	*rgb1;
	unsigned char	*rgb2;
	unsigned int	newr;
	unsigned int	newg;
	unsigned int	newb;

	rgb1 = (unsigned char*)&color1;
	rgb2 = (unsigned char*)&color2;
	newr = (rgb1[2] * (0xff - fade) + rgb2[2] * fade) / 0xff;
	newg = (rgb1[1] * (0xff - fade) + rgb2[1] * fade) / 0xff;
	newb = (rgb1[0] * (0xff - fade) + rgb2[0] * fade) / 0xff;
	return ((newr << 8 * 3) + (newg << 8 * 2) + (newb << 8 * 1) + 0xff);
}

int				skybox(t_bmp *img, t_obj *obj, t_ray r)
{
	t_cast_result	res;

	res.color = 0;
	r.pos.x = 0;
	r.pos.y = 0;
	r.pos.z = 0;
	res.texture = img;
	res.normal_map = NULL;
	for (int i = 0; i < obj->tri_amount; i++)
		if (0 < cast_face(obj->tris[i], r, &res))
		{
			face_color(res.u, res.v, obj->tris[i], &res);
			return (res.color);
		}
	return (res.color);
}

int				fog(int color, float dist, unsigned fog_color, t_level *level)
{
	float	fade;

	if (dist < level->ui.render_distance)
	{
		fade = (dist + 1) / (level->ui.render_distance - 1);
		fade = fade > 1 ? 1 : fade;
		return (crossfade(color >> 8, fog_color >> 8, 0xff * fade));
	}
	return (fog_color);
}

void			blur_pixels(unsigned *color, int gap)
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
			res = crossfade(res >> 8, col1 >> 8, fade * 0xff);
			res = crossfade(res >> 8, col2 >> 8, fade * 0xff);
			res = crossfade(res >> 8, col3 >> 8, fade * 0xff);
			res = crossfade(res >> 8, col4 >> 8, fade * 0xff);
			color[x + (y * RES_X)] = res;
			x += gap;
		}
		y += gap;
	}
}

int				smooth_color(unsigned *pixels, int gap, int x, int y)
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
		return(crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff));
	}
	if (y >= RES_Y - gap)
	{
		re1 = pixels[dx + dy * RES_X];
		re2 = pixels[dx + gap + dy * RES_X];
		return(crossfade(re1 >> 8, re2 >> 8, x % gap / (float)gap * 0xff));
	}
	re1 = pixels[dx + dy * RES_X];
	re2 = pixels[dx + (dy + gap) * RES_X];
	tmp = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff);
	re1 = pixels[dx + gap + dy * RES_X];
	re2 = pixels[dx + gap + (dy + gap) * RES_X];
	re1 = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff);
	return(crossfade(tmp >> 8, re1 >> 8, x % gap / (float)gap * 0xff));
}

void			fill_pixels(unsigned *grid, int gap, int blur, int smooth)
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

t_vec3			get_normal(int vec)
{
	unsigned char	*v;
	t_vec3			dir;

	v = (unsigned char*)&vec;
	dir.x = v[3] - 128;
	dir.y = -(v[1] - 128);
	dir.z = v[2] - 128;
	return (dir);
}

void			face_color(float u, float v, t_tri t, t_cast_result *res)
{
	int		x;
	int 	y;
	float	w;

	w = 1 - u - v;
	x =	((t.verts[0].txtr.x * res->texture->width * w +
			t.verts[1].txtr.x * res->texture->width * v +
			t.verts[2].txtr.x * res->texture->width * u) / (float)(u + v + w));
	y =	((t.verts[0].txtr.y * res->texture->height * w +
			t.verts[1].txtr.y * res->texture->height * v +
			t.verts[2].txtr.y * res->texture->height * u) / (float)(u + v + w));
	if (y >= res->texture->height)
		y = y % res->texture->height;
	else if (y < 0)
		y = -y % res->texture->height;
	y = res->texture->height - y - 1;
	if (x >= res->texture->width)
		x = x % res->texture->width;
	else if (x < 0)
		x = -x % res->texture->width;
	res->color = res->texture->image[x + (y * res->texture->width)];
	if (!res->normal_map)
		return;

	x =	((t.verts[0].txtr.x * res->normal_map->width * w +
			t.verts[1].txtr.x * res->normal_map->width * v +
			t.verts[2].txtr.x * res->normal_map->width * u) / (float)(u + v + w));
	y =	((t.verts[0].txtr.y * res->normal_map->height * w +
			t.verts[1].txtr.y * res->normal_map->height * v +
			t.verts[2].txtr.y * res->normal_map->height * u) / (float)(u + v + w));
	if (y >= res->normal_map->height)
		y = y % res->normal_map->height;
	else if (y < 0)
		y = -y % res->normal_map->height;
	y = res->normal_map->height - y - 1;
	if (x >= res->normal_map->width)
		x = x % res->normal_map->width;
	else if (x < 0)
		x = -x % res->normal_map->width;
	res->normal = get_normal(res->normal_map->image[x + (y * res->normal_map->width)]);
}
