/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaders.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 16:52:44 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/21 19:51:29by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		opacity(t_cast_result *res, t_level *l, t_obj *obj, float opacity)
{
	t_cast_result	transparent;
	t_ray			normal;

	transparent = *res;
	normal.pos = res->ray.pos;
	normal.dir = res->normal;
	vec_mult(&normal.dir, vec_dot(transparent.ray.dir, normal.dir) * l->all.tris[res->face_index].refractivity);
	vec_add(&transparent.ray.dir, transparent.ray.dir, normal.dir);
	cast_all_color(transparent.ray, l, obj, &transparent);
	res->color = crossfade((unsigned)res->color >> 8, (unsigned)transparent.color >> 8, opacity * 0xff, opacity * 0xff);
}

t_color		sunlight(t_level *l, t_cast_result *res, t_color light)
{
	unsigned	color;
	float		res_brightness;
	int			i;
	t_ray		r;

	color = 0;
	if (vec_dot(res->normal, l->ui.sun_dir) < 0)
		return (light);
	r.dir.x = l->ui.sun_dir.x;
	r.dir.y = l->ui.sun_dir.y;
	r.dir.z = l->ui.sun_dir.z;
	r.pos = res->ray.pos;
	i = 0;
	while (i < l->all.tris[res->face_index].shadow_faces->tri_amount)
	{
		if (0 < cast_face(l->all.tris[res->face_index].shadow_faces->tris[i], r, NULL))
			return (light);
		i++;
	}
	res_brightness = vec_dot(res->normal, l->ui.sun_dir);
	light.r += res_brightness * l->ui.sun_color.r;
	light.g += res_brightness * l->ui.sun_color.g;
	light.b += res_brightness * l->ui.sun_color.b;
	return (light);
}

t_color		lights(t_level *l, t_cast_result *res, t_vec3 normal)
{
	t_ray	ray;
	t_vec3	diff;
	float	dist;
	t_color	result;
	int		i;

	i = 0;
	result.r = 0;
	result.g = 0;
	result.b = 0;
	while (i < l->light_amount)
	{
		vec_sub(&diff, res->ray.pos, l->lights[i].pos);
		dist = vec_length(diff);
		ray.pos = l->lights[i].pos;
		ray.dir = diff;
		if (dist < l->lights[i].radius && vec_dot(diff, res->normal) < 0 && vec_dot(diff, normal) < 0)
		{
			if (!res->raytracing)
			{
				vec_normalize(&diff);
				result.r += (1.0 - dist / l->lights[i].radius) * l->lights[i].color.r * -vec_dot(diff, res->normal);
				result.g += (1.0 - dist / l->lights[i].radius) * l->lights[i].color.g * -vec_dot(diff, res->normal);
				result.b += (1.0 - dist / l->lights[i].radius) * l->lights[i].color.b * -vec_dot(diff, res->normal);
			}
			else if (cast_all(ray, l, NULL, NULL, NULL) >= vec_length(diff) - 0.1)
			{
				vec_normalize(&diff);
				result.r += (1.0 - dist / l->lights[i].radius) * l->lights[i].color.r * -vec_dot(diff, res->normal);
				result.g += (1.0 - dist / l->lights[i].radius) * l->lights[i].color.g * -vec_dot(diff, res->normal);
				result.b += (1.0 - dist / l->lights[i].radius) * l->lights[i].color.b * -vec_dot(diff, res->normal);
			}
		}
		i++;
	}
	result.r += l->world_brightness;
	result.g += l->world_brightness;
	result.b += l->world_brightness;
	return (result);
}

void		reflection(t_cast_result *res, t_level *l, t_obj *obj)
{
	t_cast_result	reflection;
	t_ray			normal;

	reflection = *res;
	normal.pos = res->ray.pos;
	normal.dir = res->normal;
	vec_mult(&normal.dir, vec_dot(reflection.ray.dir, normal.dir) * -2);
	vec_add(&reflection.ray.dir, reflection.ray.dir, normal.dir);
	cast_all_color(reflection.ray, l, obj, &reflection);
	res->color = crossfade((unsigned)res->color >> 8, reflection.color >> 8, l->all.tris[res->face_index].reflectivity * 0xff, (unsigned)res->color << 24 >> 24);
}

unsigned		shader_wave(t_vec3 mod, t_vec3 *normal, unsigned col1, unsigned col2)
{
	float			time;
	float			oscillation;
	float			res;

	time = SDL_GetTicks() / 1000.0;
	oscillation = (sin(time) + 1) / 2;
	float	tmp = 2 * M_PI / 6 * (sin(mod.z) / 5 + sin(mod.x) / 4 + mod.x + time);
	tmp = sin(tmp);
	res = fabs(tmp);
	normal->x -= res / 5;
	normal->y = 1;
	normal->z = 0;
	vec_normalize(normal);
	col1 = crossfade(col1, col2, res * 0xff, 0xff);
	return (col1);
}

unsigned		shader_rule30(t_vec3 pos)
{
	static char		**cells = NULL;
	static int		allocated = 0;
	static int		started = 0;
	int				size = 10000;
	int				res;

	if (!started)
	{
		started = 1;
		if (!(cells = (char **)malloc(sizeof(char*) * size)))
			ft_error("memory allocation failed");
		for (int i = 0; i < size; i++)
		{
			if (!(cells[i] = (char *)malloc(sizeof(char) * size)))
				ft_error("memory allocation failed");
			ft_bzero(cells[i], size);
		}
		cells[0][size / 2] = 1;
		for (int y = 0; y < size - 1; y++)
		{
			for (int x = 1; x < size - 1; x++)
			{
				if (cells[y][x - 1] == 1 && cells[y][x] == 1 && cells[y][x + 1] == 1)
					cells[y + 1][x] = 0;
				else if (cells[y][x - 1] == 1 && cells[y][x] == 1 && cells[y][x + 1] == 0)
					cells[y + 1][x] = 0;
				else if (cells[y][x - 1] == 1 && cells[y][x] == 0 && cells[y][x + 1] == 1)
					cells[y + 1][x] = 0;
				else if (cells[y][x - 1] == 1 && cells[y][x] == 0 && cells[y][x + 1] == 0)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 1 && cells[y][x + 1] == 1)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 1 && cells[y][x + 1] == 0)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 0 && cells[y][x + 1] == 1)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 0 && cells[y][x + 1] == 0)
					cells[y + 1][x] = 0;
				else
					cells[y + 1][x] = 0;
			}
		}
		allocated = 1;
		return (0xff00ffff);
	}
	res = 0x000000ff;
	if (allocated)
	{
		int x;
		int y;
		x = (int)pos.x + size / 2;
		y = (int)pos.z;
		if (x > 0 && x < size - 1 && y > 0 && y < size - 1)
		{
			if (cells[y][x])
				res = 0xffffffff;
		}
	}
	return (res);
}
