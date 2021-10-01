/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cast_face.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/20 12:35:22 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/16 17:33:06 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

float	cast_all(t_ray vec, t_level *level, int *index)
{
	float	res;
	float	tmp;
	int		i;

	res = FLT_MAX;
	vec_normalize(&vec.dir);
	i = 0;
	while (i < level->all.tri_amount)
	{
		tmp = cast_face(level->all.tris[i], vec, NULL);
		if (tmp > 0 && tmp < res)
		{
			res = tmp;
			if (index)
				*index = i;
		}
		i++;
	}
	return (res);
}

static void	grid_check(float *val, int isgrid)
{
	if (isgrid)
	{
		if (*val < 0)
			*val = 1 - fabs(*val - (int)*val);
		else
			*val = fabs(*val - (int)*val);
	}
}

float	cast_face(t_tri t, t_ray ray, t_cast_result *res)
{
	t_vec3	qvec;
	t_vec3	pvec;
	t_vec3	tvec;
	t_vec2	uv;
	float	invdet;

	if (res)
		res->raycast_amount++;
	vec_cross(&pvec, ray.dir, t.v0v2);
	invdet = 1 / vec_dot(pvec, t.v0v1);
	vec_sub(&tvec, ray.pos, t.verts[0].pos);
	uv.x = vec_dot(tvec, pvec) * invdet;
	grid_check(&uv.x, t.isgrid);
	if (uv.x < 0 || uv.x > 1)
		return (0);
	vec_cross(&qvec, tvec, t.v0v1);
	uv.y = vec_dot(ray.dir, qvec) * invdet;
	grid_check(&uv.y, t.isgrid);
	if (!t.isquad && (uv.y < 0 || uv.x + uv.y > 1))
		return (0);
	else if (uv.y < 0 || uv.y > 1)
		return (0);
	if (res)
		res->uv = uv;
	return (vec_dot(qvec, t.v0v2) * invdet);
}
