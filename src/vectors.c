/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vectors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/07 18:13:00 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/26 01:27:38 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		vec_normalize(float vec[3])
{
	float w = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	vec[0] /= w;
	vec[1] /= w;
	vec[2] /= w;
}

float		vec_length(float vec[3])
{
	return (sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]));
}

float		vec_dot(float ve1[3], float ve2[3])
{
	return (ve1[0] * ve2[0] + ve1[1] * ve2[1] + ve1[2] * ve2[2]);
}

int			vec_cmp(float ve1[3], float ve2[3])
{
	if (ve1[0] != ve2[0])
		return (0);
	if (ve1[1] != ve2[1])
		return (0);
	if (ve1[2] != ve2[2])
		return (0);
	return (1);
}

void		vec_avg(float res[3], float ve1[3], float ve2[3])
{
	res[0] = (ve1[0] + ve2[0]) / 2;
	res[1] = (ve1[1] + ve2[1]) / 2;
	res[2] = (ve1[2] + ve2[2]) / 2;
}

void		vec_add(float res[3], float ve1[3], float ve2[3])
{
	res[0] = ve1[0] + ve2[0];
	res[1] = ve1[1] + ve2[1];
	res[2] = ve1[2] + ve2[2];
}

void		vec_copy(float res[3], float ve[3])
{
	res[0] = ve[0];
	res[1] = ve[1];
	res[2] = ve[2];
}

void		vec2d_copy(float res[2], float ve[2])
{
	res[0] = ve[0];
	res[1] = ve[1];
}

void		vec_sub(float res[3], float ve1[3], float ve2[3])
{
	res[0] = ve1[0] - ve2[0];
	res[1] = ve1[1] - ve2[1];
	res[2] = ve1[2] - ve2[2];
}

void		vec_cross(float res[3], float u[3], float v[3])
{
	res[0] = u[1] * v[2] - v[1] * u[2];
	res[1] = v[0] * u[2] - u[0] * v[2];
	res[2] = u[0] * v[1] - v[0] * u[1];
}

void		vec_rot(float res[3], float ve1[3], float ang)
{
	float c = cos(ang);
	float s = sin(ang);
	res[0] = ve1[0] * c + ve1[2] * s;
	res[1] = ve1[1];
	res[2] = -ve1[0] * s + ve1[2] * c;
}
