/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcohen <alcohen@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/28 23:07:10 by alcohen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

unsigned int	crossfade(unsigned int color1, unsigned int color2,
							unsigned int fade, unsigned int alpha)
{
	unsigned char	*rgb1;
	unsigned char	*rgb2;
	unsigned int	newr;
	unsigned int	newg;
	unsigned int	newb;

	rgb1 = (unsigned char *)&color1;
	rgb2 = (unsigned char *)&color2;
	newr = (rgb1[2] * (0xff - fade) + rgb2[2] * fade) / 0xff;
	newg = (rgb1[1] * (0xff - fade) + rgb2[1] * fade) / 0xff;
	newb = (rgb1[0] * (0xff - fade) + rgb2[0] * fade) / 0xff;
	return ((newr << 8 * 3) + (newg << 8 * 2) + (newb << 8 * 1) + alpha);
}

unsigned int	brightness(unsigned int color1, t_color new)
{
	unsigned char	*rgb;
	unsigned int	newr;
	unsigned int	newg;
	unsigned int	newb;

	rgb = (unsigned char *)&color1;
	newr = rgb[2] * sqrt(new.r);
	newg = rgb[1] * sqrt(new.g);
	newb = rgb[0] * sqrt(new.b);
	if (newr > 0xff)
		newr = 0xff;
	if (newg > 0xff)
		newg = 0xff;
	if (newb > 0xff)
		newb = 0xff;
	if (newr < 0)
		newr = 0;
	if (newg < 0)
		newg = 0;
	if (newb < 0)
		newb = 0;
	return ((newr << 8 * 3) + (newg << 8 * 2) + (newb << 8 * 1));
}

int	skybox(t_level *l, t_cast_result res)
{
	t_color			tmp;
	t_obj			*obj;
	int				i;

	if (res.reflection_depth)
		obj = &l->sky.all;
	else
		obj = &l->sky.visible;
	if (l->skybox_brightness != 0)
	{
		tmp.r = l->skybox_brightness;
		tmp.g = l->skybox_brightness;
		tmp.b = l->skybox_brightness;
	}
	else
	{
		tmp.r = l->ui.sun_color.r + l->world_brightness;
		tmp.g = l->ui.sun_color.g + l->world_brightness;
		tmp.b = l->ui.sun_color.b + l->world_brightness;
	}
	res.color = 0;
	res.ray.pos.x = 0;
	res.ray.pos.y = 0;
	res.ray.pos.z = 0;
	res.texture = &l->sky.img;
	res.normal_map = NULL;
	res.baked = NULL;
	res.spray_overlay = NULL;
	res.raytracing = 0;
	i = 0;
	while (i < obj->tri_amount)
	{
		if (0 < cast_face(obj->tris[i], res.ray, &res))
		{
			face_color(res.uv.x, res.uv.y, obj->tris[i], &res);
			res.color = brightness(res.color >> 8, tmp) + 0xff;
			return (res.color);
		}
		i++;
	}
	return (res.color);
}

void	fog(unsigned int *color, float dist, unsigned int fog_color, t_level *level)
{
	float	fade;

	if (dist < level->ui.render_distance)
	{
		fade = (dist + 1) / (level->ui.render_distance - 1);
		if (fade > 1)
			fade = 1;
		*color = crossfade(*color >> 8, fog_color >> 8, 0xff * fade, 0xff);
	}
	else
		*color = fog_color;
}

void	blur_pixels(unsigned int *color, int gap)
{
	int		res;
	int		x;
	int		y;
	int		col;

	y = gap;
	res = 0;
	while (y < RES_Y - gap)
	{
		x = gap;
		while (x < RES_X - gap)
		{
			res = color[x + (y * RES_X)];
			col = color[x - gap + (y * RES_X)];
			res = crossfade(res >> 8, col >> 8, .25 * 0xff, 0xff);
			col = color[x + ((y - gap) * RES_X)];
			res = crossfade(res >> 8, col >> 8, .25 * 0xff, 0xff);
			col = color[x + gap + (y * RES_X)];
			res = crossfade(res >> 8, col >> 8, .25 * 0xff, 0xff);
			col = color[x + ((y + gap) * RES_X)];
			res = crossfade(res >> 8, col >> 8, .25 * 0xff, 0xff);
			color[x + (y * RES_X)] = res;
			x += gap;
		}
		y += gap;
	}
}

int	smooth_color(unsigned int *pixels, int gap, int x, int y)
{
	int	dx;
	int	dy;
	int	re1;
	int	re2;
	int	tmp;

	re1 = 0;
	re2 = 0;
	tmp = 0;
	dx = x - x % gap;
	dy = y - y % gap;
	if (x >= RES_X - gap && y >= RES_Y - gap)
		return (pixels[dx + ((y - y % gap) * RES_X)]);
	if (x >= RES_X - gap)
	{
		re1 = pixels[dx + dy * RES_X];
		re2 = pixels[dx + (dy + gap) * RES_X];
		return (crossfade(re1 >> 8, re2 >> 8, y % gap
				/ (float)gap * 0xff, 0xff));
	}
	if (y >= RES_Y - gap)
	{
		re1 = pixels[dx + dy * RES_X];
		re2 = pixels[dx + gap + dy * RES_X];
		return (crossfade(re1 >> 8, re2 >> 8, x % gap
				/ (float)gap * 0xff, 0xff));
	}
	re1 = pixels[dx + dy * RES_X];
	re2 = pixels[dx + (dy + gap) * RES_X];
	tmp = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0xff);
	re1 = pixels[dx + gap + dy * RES_X];
	re2 = pixels[dx + gap + (dy + gap) * RES_X];
	re1 = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0xff);
	return (crossfade(tmp >> 8, re1 >> 8, x % gap / (float)gap * 0xff, 0xff));
}

void	fill_pixels(unsigned int *grid, int gap, int blur, int smooth)
{
	int	color;
	int	x;
	int	y;

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

SDL_Color	get_sdl_color(unsigned int color)
{
	SDL_Color	res;

	res.r = (color << 8 * 0) >> 8 * 3;
	res.g = (color << 8 * 1) >> 8 * 3;
	res.b = (color << 8 * 2) >> 8 * 3;
	res.a = (color << 8 * 3) >> 8 * 3;
	return (res);
}

t_vec3	get_normal(int vec)
{
	unsigned char	*v;
	t_vec3			dir;

	v = (unsigned char *)&vec;
	dir.x = v[3] - 128;
	dir.y = -(v[1] - 128);
	dir.z = v[2] - 128;
	return (dir);
}

static void	wrap_coords(int *x, int *y, int max_x, int max_y)
{
	while (*y < 0)
		*y += max_y;
	if (*y >= max_y)
		*y = *y % max_y;
	*y = max_y - *y - 1;
	while (*x < 0)
		*x += max_x;
	if (*x >= max_x)
		*x = *x % max_x;
}

void	face_color(float u, float v, t_tri t, t_cast_result *res)
{
	int		x;
	int		y;
	float	w;

	w = 1 - u - v;
	x = ((t.verts[0].txtr.x * res->texture->width * w
				+ t.verts[1].txtr.x * res->texture->width * v
				+ t.verts[2].txtr.x * res->texture->width * u)
			/ (float)(u + v + w));
	y = ((t.verts[0].txtr.y * res->texture->height * w
				+ t.verts[1].txtr.y * res->texture->height * v
				+ t.verts[2].txtr.y * res->texture->height * u)
			/ (float)(u + v + w));
	wrap_coords(&x, &y, res->texture->width, res->texture->height);
	res->color = res->texture->image[x + (y * res->texture->width)];
	if (res->spray_overlay && res->spray_overlay[x + y * res->texture->width])
		res->color = res->spray_overlay[x + y * res->texture->width];
	if (res->baked && !res->raytracing)
		res->color = brightness(
				res->color >> 8, res->baked[x + y * res->texture->width])
			+ (res->color << 24 >> 24);
	if (!res->normal_map)
		return ;
	x = ((t.verts[0].txtr.x * res->normal_map->width * w
				+ t.verts[1].txtr.x * res->normal_map->width * v
				+ t.verts[2].txtr.x * res->normal_map->width * u)
			/ (float)(u + v + w));
	y = ((t.verts[0].txtr.y * res->normal_map->height * w
				+ t.verts[1].txtr.y * res->normal_map->height * v
				+ t.verts[2].txtr.y * res->normal_map->height * u)
			/ (float)(u + v + w));
	wrap_coords(&x, &y, res->normal_map->width, res->normal_map->height);
	res->normal = get_normal(
			res->normal_map->image[x + (y * res->normal_map->width)]
			);
}
