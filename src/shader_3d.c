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

static int hash[] = {208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
					 185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
					 9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
					 70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
					 203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
					 164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
					 228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
					 232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
					 193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
					 101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
					 135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
					 114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

int noise2(int x, int y)
{
	int tmp;

	tmp = hash[y % 256];
	return (hash[(tmp + x) % 256]);
}

float lin_inter(float x, float y, float s)
{
	return (x + s * (y-x));
}

float smooth_inter(float x, float y, float s)
{
	return (lin_inter(x, y, s * s * (3-2*s)));
}

float noise2d(float x, float y)
{
	int		x_int = x;
	int		y_int = y;
	float	x_frac = x - x_int;
	float	y_frac = y - y_int;
	int		s = noise2(x_int, y_int);
	int		t = noise2(x_int+1, y_int);
	int		u = noise2(x_int, y_int+1);
	int		v = noise2(x_int+1, y_int+1);
	float	low = smooth_inter(s, t, x_frac);
	float	high = smooth_inter(u, v, x_frac);
	return (smooth_inter(low, high, y_frac));
}

float   perlin_noise(float x, float y, float freq, int depth)
{
	float	xa = x * freq;
	float	ya = y * freq;
	float	amp = 1.0;
	float	fin = 0;
	float	div = 0.0;
	int		i;

	i = 0;
	while (i < depth)
	{
		div += 256 * amp;
		fin += noise2d(xa, ya) * amp;
		amp /= 2;
		xa *= 2;
		ya *= 2;
		i++;
	}
	return (fin / div);
}

void	perlin_init(t_tri *t)
{
	t->perlin = (t_perlin_settings*)malloc(sizeof(t_perlin_settings));
	if (!t->perlin)
	{
		nonfatal_error("failed to initialize perlin shader");
		return ;
	}
	t->perlin->scale = 1;
	t->perlin->depth = 6;
	t->perlin->move_speed = 1;
	t->perlin->speed_diff = 1.5;
	t->perlin->visualizer = 0;
	t->perlin->min = 0;
	t->perlin->max = 1;
	t->perlin->distance = 0;
	t->perlin->noise_opacity = 0;
	ft_memset(&t->perlin->color_1, 0, sizeof(t_color_hsl));
	ft_memset(&t->perlin->color_2, 0, sizeof(t_color_hsl));
	t->perlin->color_2.lightness = -1;
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
	float opacity;

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

unsigned int	shader_test(t_vec3 pos, t_level *level, t_cast_result *res)
{
	t_perlin_settings	p;
	t_vec2				v;
	t_vec3				ogpos;
	t_vec3				tmp;
	float				time;
	float				perlin;

	v.x = fabs(pos.x);
	v.y = fabs(pos.z);
	res->normal = level->all.tris[res->face_index].normal;
	if (!level->all.tris[res->face_index].perlin)
		return (0x000000ff);
	p = *level->all.tris[res->face_index].perlin;
	time = SDL_GetTicks() / 1000.0 * p.move_speed;

	perlin = perlin_noise(fabs(pos.x) + time * p.speed_diff,
							fabs(pos.z), p.scale, p.depth);
	perlin = stretch_value(perlin, p.min, p.max);

	float opacity;
	opacity = noise_opacity(p, perlin, *res);

	if (p.visualizer == 1)
		return (crossfade(p.color_1.color >> 8, p.color_2.color >> 8, perlin * 0xff, opacity * 0xff));
	res->normal.x += perlin - 0.5;

	ogpos = pos;
	vec_sub(&tmp, pos, level->cam.pos);
	vec_mult(&tmp, perlin);
	vec_add(&pos, ogpos, tmp);

	perlin = perlin_noise(fabs(pos.x) + time, fabs(pos.z), p.scale, p.depth);
	perlin = stretch_value(perlin, p.min, p.max);

	if (p.visualizer == 2)
	{
		if (fmod(fabs(pos.x), 4) > 2 ^ fmod(fabs(pos.z), 4) > 2)
			return (0xffffffff);
		if (level->all.tris[res->face_index].opacity)
			return (0);
		return (0x000000ff);
	}
	opacity = noise_opacity(p, perlin, *res);
	return (crossfade(p.color_1.color >> 8, p.color_2.color >> 8, perlin * 0xff, opacity * 0xff));
}
