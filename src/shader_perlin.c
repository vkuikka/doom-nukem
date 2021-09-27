/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader_3d.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/16 12:01:41 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/16 20:03:54y vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

int	noise2(int x, int y)
{
	static int	*hash = NULL;
	int			tmp;

	if (!hash)
	{
		hash = (int *)malloc(sizeof(int) * 256);
		if (!hash)
		{
			nonfatal_error("memory allocation failed for perlin noise");
			return (0);
		}
		tmp = 0;
		while (tmp < 256)
		{
			hash[tmp] = rand() % 256;
			tmp++;
		}
	}
	tmp = hash[y % 256];
	return (hash[(tmp + x) % 256]);
}

float	lin_inter(float x, float y, float s)
{
	return (x + s * (y - x));
}

float	smooth_inter(float x, float y, float s)
{
	return (lin_inter(x, y, s * s * (3 - 2 * s)));
}

float	noise2d(float x, float y)
{
	t_ivec2	in;
	t_vec2	frac;
	float	low;
	float	high;

	in.x = (int)x;
	in.y = (int)y;
	frac.x = x - in.x;
	frac.y = y - in.y;
	low = smooth_inter(noise2(in.x, in.y),
			noise2(in.x + 1, in.y), frac.x);
	high = smooth_inter(noise2(in.x, in.y + 1),
			noise2(in.x + 1, in.y + 1), frac.x);
	return (smooth_inter(low, high, frac.y));
}

float	perlin_noise(float x, float y, float freq, int depth)
{
	float	amp;
	float	fin;
	float	div;
	int		i;

	i = 0;
	fin = 0;
	div = 0;
	amp = 1;
	x *= freq;
	y *= freq;
	while (i < depth)
	{
		div += 256 * amp;
		fin += noise2d(x, y) * amp;
		amp /= 2;
		x *= 2;
		y *= 2;
		i++;
	}
	return (fin / div);
}

void	perlin_init(t_tri *t)
{
	if (!t->perlin)
	{
		t->perlin = (t_perlin_settings *)malloc(sizeof(t_perlin_settings));
		if (!t->perlin)
		{
			nonfatal_error("failed to initialize perlin shader");
			return ;
		}
		t->perlin->move_speed = 1;
		t->perlin->speed_diff = 1;
		t->perlin->scale = 1;
		t->perlin->min = 0;
		t->perlin->max = 1;
		t->perlin->resolution = 5;
		t->perlin->depth = 3;
		t->perlin->noise_opacity = 0;
		t->perlin->distance = 0;
		t->perlin->visualizer = 0;
		ft_memset(&t->perlin->color_1, 0, sizeof(t_color_hsl));
		ft_memset(&t->perlin->color_2, 0, sizeof(t_color_hsl));
		t->perlin->color_2.lightness = -1;
	}
}

float	stretch_value(float perlin, float min, float max)
{
	if (perlin > max)
		perlin = max;
	if (perlin < min)
		perlin = min;
	return ((perlin - min) * (1 / (max - min)));
}

float	noise_opacity(t_perlin_settings p, float perlin, t_cast_result res)
{
	float	opacity;

	if (!p.noise_opacity)
		opacity = 1;
	else
	{
		opacity = perlin / p.noise_opacity;
		if (opacity > 1)
			opacity = 1;
		else if (opacity < 0)
			opacity = 0;
	}
	if (!p.distance)
		return (opacity);
	if (res.dist < p.distance)
		opacity = opacity - (res.dist / p.distance);
	else
		opacity = 0;
	if (opacity < 0)
		opacity = 0;
	return (opacity);
}

float	perlin_opacity(t_vec3 *pos, float perlin, t_level *l,
						t_perlin_settings p)
{
	t_vec3	ogpos;
	t_vec3	tmp;
	float	time;

	ogpos = *pos;
	time = SDL_GetTicks() / 1000.0 * p.move_speed;
	vec_sub(&tmp, *pos, l->cam.pos);
	vec_normalize(&tmp);
	vec_mult(&tmp, perlin * p.depth);
	vec_add(pos, *pos, tmp);
	perlin = perlin_noise(fabs(pos->x) + time,
			fabs(pos->z), p.scale, p.resolution);
	perlin = stretch_value(perlin, p.min, p.max);
	return (perlin);
}

static unsigned int	depth_grid_visualizer(t_vec3 pos,
											t_level *level, t_cast_result *res)
{
	if (fmod(fabs(pos.x), 4) > 2 ^ fmod(fabs(pos.z), 4) > 2)
		return (0xffffffff);
	if (level->all.tris[res->face_index].opacity)
		return (0);
	return (0x000000ff);
}

unsigned int	shader_perlin(t_vec3 pos, t_level *level, t_cast_result *res)
{
	t_perlin_settings	p;
	float				time;
	float				perlin;
	float				opacity;

	if (!level->all.tris[res->face_index].perlin)
		return (0x000000ff);
	res->normal = level->all.tris[res->face_index].normal;
	p = *level->all.tris[res->face_index].perlin;
	time = SDL_GetTicks() / 1000.0 * p.move_speed;
	perlin = perlin_noise(fabs(pos.x) + time * p.speed_diff,
			fabs(pos.z), p.scale, p.resolution);
	perlin = stretch_value(perlin, p.min, p.max);
	opacity = noise_opacity(p, perlin, *res);
	if (p.visualizer == 1)
		return (crossfade(p.color_1.color >> 8,
				p.color_2.color >> 8, perlin * 0xff, opacity * 0xff));
	res->normal.x += perlin - 0.5;
	perlin = perlin_opacity(&pos, perlin, level, p);
	if (p.visualizer == 2)
		return (depth_grid_visualizer(pos, level, res));
	opacity = noise_opacity(p, perlin, *res);
	return (crossfade(p.color_1.color >> 8,
			p.color_2.color >> 8, perlin * 0xff, opacity * 0xff));
}
