/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaders.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/31 16:52:44 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/24 18:38:27 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	opacity(t_cast_result *res, t_level *l, t_obj *obj, float opacity)
{
	t_cast_result	transparent;
	t_ray			normal;

	transparent = *res;
	if (l->all.tris[res->face_index].refractivity == 0)
		cast_all_color(l, obj, &transparent);
	else
	{
		normal.pos = res->ray.pos;
		normal.dir = res->normal;
		vec_mult(&normal.dir, vec_dot(transparent.ray.dir, normal.dir)
			* l->all.tris[res->face_index].refractivity);
		vec_add(&transparent.ray.dir, transparent.ray.dir, normal.dir);
		cast_all_color(l, l->all.tris[res->face_index].opacity_obj_all,
			&transparent);
	}
	res->color = crossfade((unsigned int)res->color >> 8,
			(unsigned int)transparent.color >> 8,
			opacity * 0xff, opacity * 0xff);
}

t_color	sunlight(t_level *l, t_cast_result *res, t_color light)
{
	unsigned int	color;
	float			res_brightness;
	int				i;
	t_ray			r;

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
		if (0 < cast_face(l->all.tris[res->face_index].shadow_faces->tris[i],
				r, NULL))
			return (light);
		i++;
	}
	res_brightness = vec_dot(res->normal, l->ui.sun_dir);
	light.r += res_brightness * l->ui.sun_color.r;
	light.g += res_brightness * l->ui.sun_color.g;
	light.b += res_brightness * l->ui.sun_color.b;
	return (light);
}

static void	color_set(t_color *col, float value)
{
	col->r = value;
	col->g = value;
	col->b = value;
}

t_color	lights(t_level *l, t_cast_result *res, t_vec3 normal)
{
	t_ray	ray;
	t_vec3	diff;
	float	dist;
	t_color	col;
	int		i;

	i = -1;
	color_set(&col, l->world_brightness);
	while (++i < l->light_amount)
	{
		vec_sub(&diff, res->ray.pos, l->lights[i].pos);
		dist = 1.0 - vec_length(diff) / l->lights[i].radius;
		ray.pos = l->lights[i].pos;
		ray.dir = diff;
		if (dist > 0 && vec_dot(diff, res->normal) < 0
			&& vec_dot(diff, normal) < 0 && (!res->raytracing
				|| cast_all(ray, l, NULL) >= vec_length(diff) - 0.1))
		{
			vec_normalize(&diff);
			col.r += dist * l->lights[i].color.r * -vec_dot(diff, res->normal);
			col.g += dist * l->lights[i].color.g * -vec_dot(diff, res->normal);
			col.b += dist * l->lights[i].color.b * -vec_dot(diff, res->normal);
		}
	}
	return (col);
}

void	reflection(t_cast_result *res, t_level *l, t_obj *obj)
{
	t_cast_result	reflection;
	t_ray			normal;

	reflection = *res;
	normal.pos = res->ray.pos;
	normal.dir = res->normal;
	vec_mult(&normal.dir, vec_dot(reflection.ray.dir, normal.dir) * -2);
	vec_add(&reflection.ray.dir, reflection.ray.dir, normal.dir);
	cast_all_color(l, obj, &reflection);
	res->color = crossfade((unsigned int)res->color >> 8, reflection.color >> 8,
			l->all.tris[res->face_index].reflectivity * 0xff,
			(unsigned int)res->color << 24 >> 24);
}

unsigned int	shader_wave(t_vec3 mod, t_vec3 *normal,
							unsigned int col1, unsigned int col2)
{
	float	time;
	float	oscillation;
	float	res;
	float	tmp;

	time = SDL_GetTicks() / 1000.0;
	oscillation = (sin(time) + 1) / 2;
	tmp = 2 * M_PI / 6 * (sin(mod.z) / 5 + sin(mod.x) / 4 + mod.x + time);
	tmp = sin(tmp);
	res = fabs(tmp);
	normal->x -= res / 5;
	normal->y = 1;
	normal->z = 0;
	vec_normalize(normal);
	col1 = crossfade(col1, col2, res * 0xff, 0xff);
	return (col1);
}

unsigned int	shader_rule30(t_vec3 pos)
{
	static char	**cells = NULL;
	static int	allocated = 0;
	static int	started = 0;
	int			size;
	int			res;
	int			x;
	int			y;

	size = 10000;
	if (!started)
	{
		started = 1;
		cells = (char **)malloc(sizeof(char *) * size);
		if (!cells)
			ft_error("memory allocation failed");
		x = 0;
		while (x < size)
		{
			cells[x] = (char *)malloc(sizeof(char) * size);
			if (!cells[x])
				ft_error("memory allocation failed");
			ft_bzero(cells[x], size);
			x++;
		}
		cells[0][size / 2] = 1;
		y = -1;
		while (++y < size - 1)
		{
			x = 0;
			while (++x < size - 1)
			{
				if (cells[y][x - 1] == 1 && cells[y][x] == 1 &&
					cells[y][x + 1] == 1)
					cells[y + 1][x] = 0;
				else if (cells[y][x - 1] == 1 && cells[y][x] == 1 &&
							cells[y][x + 1] == 0)
					cells[y + 1][x] = 0;
				else if (cells[y][x - 1] == 1 && cells[y][x] == 0 &&
							cells[y][x + 1] == 1)
					cells[y + 1][x] = 0;
				else if (cells[y][x - 1] == 1 && cells[y][x] == 0 &&
							cells[y][x + 1] == 0)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 1 &&
							cells[y][x + 1] == 1)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 1 &&
							cells[y][x + 1] == 0)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 0 &&
							cells[y][x + 1] == 1)
					cells[y + 1][x] = 1;
				else if (cells[y][x - 1] == 0 && cells[y][x] == 0 &&
							cells[y][x + 1] == 0)
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
