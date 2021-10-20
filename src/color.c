/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 17:32:09 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/20 22:04:15 by vkuikka          ###   ########.fr       */
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
	return ((newr << 8 * 3) + (newg << 8 * 2) + (newb << 8 * 1));
}

static t_color	get_skybox_brightness(t_level *l)
{
	t_color	tmp;

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
	return (tmp);
}

static void	skybox_reset_result(t_level *l, t_cast_result *res)
{
	res->ray.pos = (t_vec3){0, 0, 0};
	res->texture = &l->sky.img;
	res->normal_map = NULL;
	res->baked = NULL;
	res->spray_overlay = NULL;
	res->raytracing = 0;
}

static float	skybox_sun_fade(t_color *col, t_cast_result *res, t_level *l)
{
	float	fade;

	if (!l->ui.sun_color.r && !l->ui.sun_color.g && !l->ui.sun_color.b)
		return (0);
	fade = vec_dot(l->ui.sun_dir, res->ray.dir);
	if (fade < SUN_SIZE)
		return (0);
	fade = fade - SUN_SIZE;
	fade = fade / (1.0 - SUN_SIZE);
	fade = (sin(fade * M_PI - M_PI / 2) + 1) / 2;
	col->r = l->ui.sun_color.r;
	col->g = l->ui.sun_color.g;
	col->b = l->ui.sun_color.b;
	res->light.r += col->r;
	res->light.g += col->g;
	res->light.b += col->b;
	return (fade);
}

void	skybox(t_level *l, t_cast_result *res)
{
	t_color	b;
	t_color	sun;
	t_obj	*obj;
	float	fade;
	int		i;

	obj = &l->sky.visible;
	if (res->reflection_depth)
		obj = &l->sky.all;
	b = get_skybox_brightness(l);
	skybox_reset_result(l, res);
	fade = skybox_sun_fade(&sun, res, l);
	i = 0;
	while (i < obj->tri_amount && 0 >= cast_face(obj->tris[i], res->ray, res))
		i++;
	if (i == obj->tri_amount)
		return ;
	face_color(res->uv.x, res->uv.y, obj->tris[i], res);
	if (fade)
		res->color = crossfade(brightness(res->color >> 8, b) >> 8,
				color_to_int(sun) >> 8, fade * 0xff, 0xff);
	else
		res->color = brightness(res->color >> 8, b) + 0xff;
}

void	fog(unsigned int *color, float dist, unsigned int fog_color,
													t_level *level)
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

static int	smooth_color_kernel(unsigned int *pixels, int gap, int x, int y)
{
	int	dx;
	int	dy;
	int	re1;
	int	re2;
	int	tmp;

	dx = x - x % gap;
	dy = y - y % gap;
	re1 = pixels[dx + dy * RES_X];
	re2 = pixels[dx + (dy + gap) * RES_X];
	tmp = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0xff);
	re1 = pixels[dx + gap + dy * RES_X];
	re2 = pixels[dx + gap + (dy + gap) * RES_X];
	re1 = crossfade(re1 >> 8, re2 >> 8, y % gap / (float)gap * 0xff, 0xff);
	return (crossfade(tmp >> 8, re1 >> 8, x % gap / (float)gap * 0xff, 0xff));
}

int	smooth_color(unsigned int *pixels, int gap, int x, int y)
{
	int	dx;
	int	dy;
	int	re1;
	int	re2;

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
	return (smooth_color_kernel(pixels, gap, x, y));
}

static void	fill_one_pixel(unsigned int *grid, int gap, t_ivec2 coord, int s)
{
	t_ivec2	limit;
	t_ivec2	i;
	int		color;

	color = grid[coord.x + coord.y * RES_X];
	limit.x = coord.x + gap;
	limit.y = coord.y + gap;
	if (limit.x > RES_X)
		limit.x -= limit.x - RES_X;
	if (limit.y > RES_Y)
		limit.y -= limit.y - RES_Y;
	i.x = coord.x;
	while (i.x < limit.x)
	{
		i.y = coord.y;
		while (i.y < limit.y)
		{
			if (s)
				grid[i.x + i.y * RES_X] = smooth_color(grid, gap, i.x, i.y);
			else
				grid[i.x + i.y * RES_X] = color;
			i.y++;
		}
		i.x++;
	}
}

void	fill_pixels(unsigned int *grid, int gap, int smooth)
{
	t_ivec2	i;

	i.y = 0;
	while (i.y < RES_Y)
	{
		i.x = 0;
		while (i.x < RES_X)
		{
			fill_one_pixel(grid, gap, i, smooth);
			i.x += gap;
		}
		i.y += gap;
	}
}

unsigned int	vec_to_color(t_vec3 color)
{
	unsigned int	res;
	unsigned char	*rgb;

	rgb = (unsigned char *)&res;
	color.x = clamp(color.x, 0, 1);
	color.y = clamp(color.y, 0, 1);
	color.z = clamp(color.z, 0, 1);
	rgb[3] = color.x * 0xff;
	rgb[2] = color.y * 0xff;
	rgb[1] = color.z * 0xff;
	rgb[0] = 0xff;
	return (res);
}

t_vec3	color_to_vec(unsigned int color)
{
	unsigned char	*rgb;
	t_vec3			res;

	rgb = (unsigned char *)&color;
	res.x = rgb[3] / (float)0xff;
	res.y = rgb[2] / (float)0xff;
	res.z = rgb[1] / (float)0xff;
	return (res);
}

static unsigned int	sharpen_kernel(unsigned int *buf, int i,
						float center, float neighbor)
{
	t_vec3	tmp;
	t_vec3	c;

	c = (t_vec3){0, 0, 0};
	tmp = color_to_vec(buf[i + RES_X]);
	vec_mult(&tmp, neighbor);
	vec_add(&c, c, tmp);
	tmp = color_to_vec(buf[i - 1]);
	vec_mult(&tmp, neighbor);
	vec_add(&c, c, tmp);
	tmp = color_to_vec(buf[i]);
	vec_mult(&tmp, center);
	vec_add(&c, c, tmp);
	tmp = color_to_vec(buf[i + 1]);
	vec_mult(&tmp, neighbor);
	vec_add(&c, c, tmp);
	tmp = color_to_vec(buf[i - RES_X]);
	vec_mult(&tmp, neighbor);
	vec_add(&c, c, tmp);
	return (vec_to_color(c));
}

void	sharpen(unsigned int *pixels, unsigned int *buf, float amount)
{
	float	neighbor;
	float	center;
	int		x;
	int		y;

	neighbor = amount * -1.0;
	center = amount * 4.0 + 1.0;
	ft_memcpy(buf, pixels, sizeof(int) * RES_X * RES_Y);
	y = 1;
	while (y < RES_Y - 1)
	{
		x = 1;
		while (x < RES_X - 1)
		{
			pixels[x + y * RES_X]
				= sharpen_kernel(buf, x + y * RES_X, center, neighbor);
			x++;
		}
		y++;
	}
}

void	post_process(t_window *window, t_level *level)
{
	if (level->ui.blur)
		blur_pixels(window->frame_buffer, level->ui.raycast_quality);
	if (level->ui.bloom_intensity)
		bloom(level, window);
	if (level->ui.ssao_radius)
		ssao(window, level);
	fill_pixels(window->frame_buffer, level->ui.raycast_quality,
		level->ui.smooth_pixels);
	if (level->ui.chromatic_abberation)
		chromatic_abberation(window->frame_buffer, window->post_process_buf,
			level->ui.chromatic_abberation);
	if (level->ui.sharpen)
		sharpen(window->frame_buffer, window->post_process_buf,
			level->ui.sharpen);
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

static void	wrap_coords_inverted(int *x, int *y, int max_x, int max_y)
{
	if (*y < 0)
		*y = max_y - (*y % max_y);
	if (*y >= max_y)
		*y = *y % max_y;
	*y = max_y - *y - 1;
	if (*x < 0)
		*x = max_x - (*x % max_x);
	if (*x >= max_x)
		*x = *x % max_x;
}

static void	normal_map(float u, float v, t_tri t, t_cast_result *res)
{
	int		x;
	int		y;
	float	w;

	w = 1 - u - v;
	x = ((t.verts[0].txtr.x * res->normal_map->width * w
				+ t.verts[1].txtr.x * res->normal_map->width * v
				+ t.verts[2].txtr.x * res->normal_map->width * u)
			/ (float)(u + v + w));
	y = ((t.verts[0].txtr.y * res->normal_map->height * w
				+ t.verts[1].txtr.y * res->normal_map->height * v
				+ t.verts[2].txtr.y * res->normal_map->height * u)
			/ (float)(u + v + w));
	wrap_coords_inverted(&x, &y, res->normal_map->width,
		res->normal_map->height);
	res->normal
		= get_normal(res->normal_map->image[x + (y * res->normal_map->width)]);
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
	wrap_coords_inverted(&x, &y, res->texture->width, res->texture->height);
	res->color = res->texture->image[x + (y * res->texture->width)];
	if (res->spray_overlay && res->spray_overlay[x + y * res->texture->width])
		res->color = res->spray_overlay[x + y * res->texture->width];
	if (res->baked && !res->raytracing && !t.isgrid && t.shader == SHADER_NONE)
		res->color = brightness(
				res->color >> 8, res->baked[x + y * res->texture->width])
			+ (res->color << 24 >> 24);
	if (res->normal_map)
		normal_map(u, v, t, res);
	else
		res->normal = t.normal;
}
