/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vectors.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/07 18:13:00 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/10 19:43:34 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

float	lerp(float a, float b, float f)
{
	return (a + f * (b - a));
}

t_vec3	vec_interpolate(t_vec3 a, t_vec3 b, float f)
{
	t_vec3	res;

	res.x = lerp(a.x, b.x, f);
	res.y = lerp(a.y, b.y, f);
	res.z = lerp(a.z, b.z, f);
	return (res);
}

float		Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

void		vec_normalize(t_vec3 *vec)
{
	float	tmp = vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
	_mm_store_ss(&tmp, _mm_rsqrt_ss(_mm_load_ss(&tmp)));
	vec_mult(vec, tmp);

	// float w = Q_rsqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	// vec->x *= pOut;
	// vec->y *= pOut;
	// vec->z *= pOut;

	// float w = sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	// vec->x /= w;
	// vec->y /= w;
	// vec->z /= w;
}

float		vec_length(t_vec3 vec)
{
	float	tmp = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
	_mm_store_ss(&tmp, _mm_sqrt_ss(_mm_load_ss(&tmp)));
	return (tmp);
	// return (sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

float		vec2_length(t_vec2 vec)
{
	return (sqrt(vec.x * vec.x + vec.y * vec.y));
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

void		vec2_avg(t_vec2 *res, t_vec2 ve1, t_vec2 ve2)
{
	res->x = (ve1.x + ve2.x) / 2;
	res->y = (ve1.y + ve2.y) / 2;
}

void		vec_avg(t_vec3 *res, t_vec3 ve1, t_vec3 ve2)
{
	res->x = (ve1.x + ve2.x) / 2;
	res->y = (ve1.y + ve2.y) / 2;
	res->z = (ve1.z + ve2.z) / 2;
}

void		vec2_add(t_vec2 *res, t_vec2 ve1, t_vec2 ve2)
{
	res->x = ve1.x + ve2.x;
	res->y = ve1.y + ve2.y;
}

void		vec2_mult(t_vec2 *res, float mult)
{
	res->x *= mult;
	res->y *= mult;
}

void		vec_add(t_vec3 *res, t_vec3 ve1, t_vec3 ve2)
{
	res->x = ve1.x + ve2.x;
	res->y = ve1.y + ve2.y;
	res->z = ve1.z + ve2.z;
}

void		vec2_sub(t_vec2 *res, t_vec2 ve1, t_vec2 ve2)
{
	res->x = ve1.x - ve2.x;
	res->y = ve1.y - ve2.y;
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

float		vec_angle(t_vec3 v1, t_vec3 v2)
{
	float	dot;
	float	len;

	dot = vec_dot(v1, v2);
	len = vec_length(v1) * vec_length(v2);
	return (acos(dot / len));
}

void		vec_mult(t_vec3 *res, float mult)
{
	res->x = res->x * mult;
	res->y = res->y * mult;
	res->z = res->z * mult;
}

void		vec_div(t_vec3 *res, float div)
{
	res->x = res->x / div;
	res->y = res->y / div;
	res->z = res->z / div;
}
