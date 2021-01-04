/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_test.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/05 01:16:41by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

float	rt_sphere(t_window *window, float ray[3])
{
	float	rsp[3];
	rsp[0] = 0;	//sphere pos - ray start pos
	rsp[1] = 0;
	rsp[2] = 5;

	float a = vec_dot(ray, ray);
	float b = 2 * vec_dot(ray, rsp);
	float c = vec_dot(rsp, rsp) - 10;

	float disc = b * b - 4 * a * c;

	float x0;
	float x1;

	if (disc < 0)
		return (0);
	else if (disc == 0)
		x0 = x1 = - 0.5 * b / a; 
    else
	{ 
        float q = (b > 0) ? 
            -0.5 * (b + sqrt(disc)) : 
            -0.5 * (b - sqrt(disc)); 
        x0 = q / a; 
        x1 = c / q; 
    } 
	if (x0 > x1)
		x1 = x0;

	if (x1 > 0)
		x1 = 0;
	x1 *= -1.0;

	if (!x1)
		SDL_SetRenderDrawColor(window->SDLrenderer, 0, 0, 0, 255);
	else
		SDL_SetRenderDrawColor(window->SDLrenderer, 255.0 / x1, 255.0 / x1, 255.0 / x1, 255);

	return (x1);
	// return ((-b - sqrt(disc)) / (2.0 * a));
}

float	rt_tri(t_window *window, t_tri t, float ray[3], float cam[3], int mid)
{

	float	pvec[3];
	vec_cross(pvec, ray, t.v0v2);
    float det = vec_dot(pvec, t.v0v1); 
	// if (det > 0)	//if backface culling is not needed edit this
	// {
	// 	SDL_SetRenderDrawColor(window->SDLrenderer, 0, 0, 0, 255);
	// 	return 0;
	// }

	float invdet = 1 / det;

	float	tvec[3];
	vec_sub(tvec, cam, t.verts[0].pos);
	float u = vec_dot(tvec, pvec) * invdet;
	if (u < 0 || u > 1)
		return 0;

	float qvec[3];
	vec_cross(qvec, tvec, t.v0v1);
	float v = vec_dot(ray, qvec) * invdet;
	// if (v < 0 || v > 1) //quad hack
	if (v < 0 || u + v > 1)
		return 0;
    float dist = vec_dot(qvec, t.v0v2) * invdet;

	SDL_SetRenderDrawColor(window->SDLrenderer, u * 255, v * 255, (1 - u - v) * 255, 255);
	return dist;

}

void	rt_test(t_window *window, float posz, float posx, float angle)
{
	t_tri	test;
	float	ray_increment_x = 1 / RES_X;
	float	ray_increment_y = 1 / RES_Y;
	float	cam[3];
	float	ray[3];
	// vec_rot(ray, (float[3]){1, 1, 1}, angle);
	// printf("%f %f %f\n", ray[0], ray[1], ray[2]);

	test.verts[0].pos[0] = -0.5;
	test.verts[0].pos[1] = -0.5;
	test.verts[0].pos[2] = 8;

	test.verts[1].pos[0] = 0.5;
	test.verts[1].pos[1] = -0.5;
	test.verts[1].pos[2] = 8;

	test.verts[2].pos[0] = 0;
	test.verts[2].pos[1] = 0.5;
	test.verts[2].pos[2] = 8;

	cam[0] = posx;
	cam[1] = 0;
	cam[2] = posz;

	vec_sub(test.v0v1, test.verts[1].pos, test.verts[0].pos);
	vec_sub(test.v0v2, test.verts[2].pos, test.verts[0].pos);

	// ray.dir[0] = world.view->dir[0] - 1/RES_X * RES_X / 2;

	for (int x = 0; x < RES_X; x++)
	{
		float tmp[3];
		tmp[0] = 1 / RES_X * x - 0.5;
		tmp[1] = 0;
		tmp[2] = 1;
		// vec_normalize(tmp);
		for (int y = 0; y < RES_Y; y++)
		{
			vec_rot(ray, tmp, angle);
			ray[1] = 1 / RES_Y * y - 0.5;
			float closest = -1;
			float dist = 0;
			// if (x % 2 ^ y % 2)
			for (int i = 0; i < 1; i++)
			{
				test.verts[0].pos[0] = (float)(i - 5) / 4;
				test.verts[0].pos[2] = 5;

				vec_sub(test.v0v1, test.verts[1].pos, test.verts[0].pos);
				vec_sub(test.v0v2, test.verts[2].pos, test.verts[0].pos);
				dist = rt_tri(window, test, ray, cam, x == RES_X / 2 && y == RES_Y / 2);
				if (!dist)
					SDL_SetRenderDrawColor(window->SDLrenderer, 0, 0, 0, 255);
				else if (dist < closest || closest == -1)
				{
					SDL_RenderDrawPoint(window->SDLrenderer, x, y);
					closest = dist;
				}
			}
		}
	}
	// printf("%f %f %f\n", test.verts[0].pos[0], test.verts[0].pos[1], test.verts[0].pos[2]);
}
