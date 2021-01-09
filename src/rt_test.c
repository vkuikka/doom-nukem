/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_test.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/09 02:40:01 by vkuikka          ###   ########.fr       */
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

float	rt_tri(t_window *window, t_tri t, float ray[3], float cam[3], int x, int y, int *col)
{

	float	pvec[3];
	vec_cross(pvec, ray, t.v0v2);
    float det = vec_dot(pvec, t.v0v1); 
	if (det > 0)	//if backface culling is not needed edit this
		return 0;

	// float invdet = 1 / vec_dot(pvec, t.v0v1);	//save a variable if culling is not used
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

	// SDL_SetRenderDrawColor(window->SDLrenderer, u * 255, v * 255, (1 - u - v) * 255, 255);
	// return (((int)(u * 255) & 0xff) << 16) + (((int)(v * 255) & 0xff) << 8) + (((int)((1-u-v) * 255) & 0xff));

	*col =	(((int)(u * 255) & 0xff) << 24) +
			(((int)(v * 255) & 0xff) << 16) +
			(((int)((1-u-v) * 255) & 0xff) << 8) + (0xff);

	return dist;
}

void	*rt_test(void *tp)
{
	t_rthread	*t = tp;
	t_tri		test;
	float		ray_increment_x = 1 / RES_X;
	float		ray_increment_y = 1 / RES_Y;
	float		cam[3];
	float		ray[3];

	test.verts[0].pos[0] = -0.5;
	test.verts[0].pos[1] = -0.5;
	test.verts[0].pos[2] = 8;

	test.verts[1].pos[0] = 0.5;
	test.verts[1].pos[1] = -0.5;
	test.verts[1].pos[2] = 8;

	test.verts[2].pos[0] = 0;
	test.verts[2].pos[1] = 0.5;
	test.verts[2].pos[2] = 8;

	cam[0] = t->player->posx;
	cam[1] = 0;
	cam[2] = t->player->posz;

	vec_sub(test.v0v1, test.verts[1].pos, test.verts[0].pos);
	vec_sub(test.v0v2, test.verts[2].pos, test.verts[0].pos);

	// ray.dir[0] = world.view->dir[0] - 1/RES_X * RES_X / 2;
	float angle = t->player->angle;
	t_window *window = t->window;
	// for (int x = 0; x < RES_X; x++)
	for (int x = t->id; x < RES_X; x += THREAD_AMOUNT)
	{
		float tmp[3];
		tmp[0] = 1 / RES_X * x - 0.5;
		tmp[1] = 0;
		tmp[2] = 1;
		// vec_normalize(tmp);
		for (int y = 0; y < RES_Y; y++)
		{
			window->depth_buffer[x + (y * (int)RES_X)] = 0;
			window->frame_buffer[x + (y * (int)RES_X)] = 0x00000000;
			// if ((!(x % 3) && !(y % 3)))
			// if (!(x % 2 ^ y % 2))		//further optimizations dont delete
			{
				vec_rot(ray, tmp, angle);
				ray[1] = 1 / RES_Y * y - 0.5;
				// float closest = -1;
				for (int j = 0; j < 10; j++)	//adjust amount of faces drawn
				{
					int color;
					float dist;

					test.verts[0].pos[2] = (float)j / 2 + 5;
					test.verts[1].pos[2] = (float)j / 2 + 5;	// moves faces out so they arent in the same spot
					test.verts[2].pos[2] = (float)j / 2 + 5;

					vec_sub(test.v0v1, test.verts[1].pos, test.verts[0].pos);
					vec_sub(test.v0v2, test.verts[2].pos, test.verts[0].pos);
					dist = rt_tri(window, test, ray, cam, x, y, &color);
					if (dist > 0 && (dist < window->depth_buffer[x + (y * (int)RES_X)] ||
								window->depth_buffer[x + (y * (int)RES_X)] == 0))
					{
						window->depth_buffer[x + (y * (int)RES_X)] = dist;
						window->frame_buffer[x + (y * (int)RES_X)] = color;
						// window->frame_buffer[(x + 1) + (y * (int)RES_X)] = color;	//part of the above optimization
						// window->frame_buffer[x + ((y + 1) * (int)RES_X)] = color;


						// window->frame_buffer[(x + 2) + (y * (int)RES_X)] = color;
						// window->frame_buffer[(x + 1) + ((y + 1) * (int)RES_X)] = color;
						// window->frame_buffer[(x + 2) + ((y + 1) * (int)RES_X)] = color;
						// window->frame_buffer[x + ((y + 2) * (int)RES_X)] = color;
						// window->frame_buffer[(x + 1) + ((y + 2) * (int)RES_X)] = color;
						// window->frame_buffer[(x + 2) + ((y + 2) * (int)RES_X)] = color;
						// closest = dist;
					}
				}
			}
		}
	}

	// SDL_SetRenderDrawColor(window->SDLrenderer, 255, 0, 0, 255);
	// SDL_RenderDrawPoint(window->SDLrenderer, RES_X / 2 + posx * 10, RES_Y / 2 - posz * 10);
	// printf("%f %f %f\n", test.verts[0].pos[0], test.verts[0].pos[1], test.verts[0].pos[2]);

	// free(t->player);
	// free(tp);
	return (NULL);
}
