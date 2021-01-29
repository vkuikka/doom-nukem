/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vectors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/07 18:13:00 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/29 03:42:09 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		vec_normalize(t_vec3 *vec)
{
	float w = sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	vec->x /= w;
	vec->y /= w;
	vec->z /= w;
}

float		vec_length(t_vec3 vec)
{
	return (sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

float		vec_dot(t_vec3 ve1, t_vec3 ve2)
{
	return (ve1.x * ve2.x + ve1.y * ve2.y + ve1.z * ve2.z);
}

int			vec_cmp(t_vec3 ve1, t_vec3 ve2)
{
	if (ve1.x != ve2.x)
		return (0);
	if (ve1.y != ve2.y)
		return (0);
	if (ve1.z != ve2.z)
		return (0);
	return (1);
}

void		vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2)
{
	res->x = (ve1.x + ve2.x) / 2;
	res->y = (ve1.y + ve2.y) / 2;
	res->z = (ve1.z + ve2.z) / 2;
}

void		vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2)
{
	res->x = ve1.x + ve2.x;
	res->y = ve1.y + ve2.y;
	res->z = ve1.z + ve2.z;
}

void		vec_copy(t_vec3 *res, t_vec3 ve)
{
	res->x = ve.x;
	res->y = ve.y;
	res->z = ve.z;
}

void		vec2_copy(t_vec2 *res, t_vec2 ve)
{
	res->x = ve.x;
	res->y = ve.y;
}

void		vec_sub(t_vec3 *res, t_vec3 ve1, t_vec3 ve2)
{
	res->x = ve1.x - ve2.x;
	res->y = ve1.y - ve2.y;
	res->z = ve1.z - ve2.z;
}

void		vec_cross(t_vec3 *res, t_vec3 u, t_vec3 v)
{
	res->x = u.y * v.z - v.y * u.z;
	res->y = v.x * u.z - u.x * v.z;
	res->z = u.x * v.y - v.x * u.y;
}

void		vec_rot(t_vec3 *res, t_vec3 ve1, float ang)
{
	float c = cos(ang);
	float s = sin(ang);
	res->x = ve1.x * c + ve1.z * s;
	res->y = ve1.y;
	res->z = -ve1.x * s + ve1.z * c;
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
