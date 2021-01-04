/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vectors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/07 18:13:00 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/05 01:39:26 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		vec_normalize_two(float vec1[3], float vec2[3])
{
	int			i;
	float		min;
	float		max;
	float		diff;

	i = 0;
	min = vec1[0] < vec2[0] ? vec1[0] : vec2[0];
	max = vec1[0] > vec2[0] ? vec1[0] : vec2[0];
	while (++i < 3)
	{
		if (vec1[i] < min)
			min = vec1[i];
		if (vec2[i] < min)
			min = vec1[i];
		if (vec1[i] > max)
			max = vec1[i];
		if (vec2[i] > max)
			max = vec2[i];
	}
	i = -1;
	diff = max - min;
	if (!diff)
		diff = 1.0;
	while (++i < 3)
	{
		vec1[i] = (vec1[i] - min) / diff;
		vec2[i] = (vec2[i] - min) / diff;
	}
}

void		vec_normalize(float vec1[3])
{
	int			i;
	float		min;
	float		max;
	float		diff;

	i = 1;
	min = vec1[0];
	max = vec1[0];
	while (i < 3)
	{
		if (vec1[i] < min)
			min = vec1[i];
		if (vec1[i] > max)
			max = vec1[i];
		i++;
	}
	i = 0;
	diff = max - min;
	if (!diff)
	{
		diff = 0.001;
		// printf("%f %f %f\n", vec1[0], vec1[1], vec1[2]);
	}
	while (i < 3)
	{
		vec1[i] = (vec1[i] - min) / diff;
		i++;
	}
}

float		vec_length(float vec[3])
{
	float	len;
	int		i;

	i = 0;
	while (i < 3)
	{
		len += vec[i] * vec[i];
		i++;
	}
	len = sqrt(len);
	return (len);
}

float		vec_dot(float ve1[3], float ve2[3])
{
	return (ve1[0] * ve2[0] + ve1[1] * ve2[1] + ve1[2] * ve2[2]);
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

// int main(void)
// {
// 	float ve1[3];
// 	float ve2[3];

// 	ve1[0] = 0;
// 	ve1[1] = 0.1;
// 	ve1[2] = 4;

// 	ve2[0] = 6;
// 	ve2[1] = 8;
// 	ve2[2] = 10;
// 	ft_normalize_two(ve1, ve2);
// 	for (int i = 0; i < 3; i++)
// 	{
// 		printf("%f %f", ve1[i], ve2[i]);
// 		printf("\n");
// 	}
// }
