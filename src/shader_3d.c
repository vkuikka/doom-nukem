/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader_3d.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/16 12:01:41 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/16 12:02:29 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static float noise(int x, int y)
{
    int n = x + y * 57;
    n = (n<<13) ^ n;
    return (1.0 - ( (n * ((n * n * 15731) + 789221) +  1376312589) & 0x7fffffff) / 1073741824.0);
}

static float cosine_interpolate(float a, float b, float x)
{
    float ft = x * M_PI;
    float f = (1 - cos(ft)) * 0.5;
    float result =  a*(1-f) + b*f;
    return result;
}

static float smooth_noise_2D(float x, float y)
{  
    float corners = ( noise(x-1, y-1)+noise(x+1, y-1)+noise(x-1, y+1)+noise(x+1, y+1) ) / 16;
    float sides   = ( noise(x-1, y)  +noise(x+1, y)  +noise(x, y-1)  +noise(x, y+1) ) /  8;
    float center  =  noise(x, y) / 4;

    return corners + sides + center;
}

static float interpolated_noise(float x, float y)
{
    int x_whole = (int) x;
    float x_frac = x - x_whole;

    int y_whole = (int) y;
    float y_frac = y - y_whole;

    float v1 = smooth_noise_2D(x_whole, y_whole); 
    float v2 = smooth_noise_2D(x_whole, y_whole+1); 
    float v3 = smooth_noise_2D(x_whole+1, y_whole); 
    float v4 = smooth_noise_2D(x_whole+1, y_whole+1); 

    float i1 = cosine_interpolate(v1,v3,x_frac);
    float i2 = cosine_interpolate(v2,v4,x_frac);

    return cosine_interpolate(i1, i2, y_frac);
}


float perlin_noise_2D(float x, float y)
{
    int octaves=5;
    float persistence=0.5;
    float total = 0;

    for(int i=0; i<octaves-1; i++)
    {
        float frequency = pow(2,i);
        float amplitude = pow(persistence,i);
        total = total + interpolated_noise(x * frequency, y * frequency) * amplitude;
    }
    return total;
}

unsigned int	shader_test(t_vec3 pos, t_level *level, t_cast_result res)
{
	t_vec2	v;
	t_vec3	ogpos;
	t_vec3	tmp;

	v.x = fabs(pos.x);
	v.y = fabs(pos.z);

	float	time;
	time = SDL_GetTicks() / 1000.0;

	float perlin = perlin_noise_2D(v.x + time, v.y + time);
	// float perlin = perlin_noise_2D(v.x, v.y);
	perlin = (perlin + 1) / 2;

	// return (crossfade(0, 0xffffff, res * 0xff, 0xff));

	tmp = pos;
	vec_normalize(&tmp);

	vec_sub(&pos, pos, level->cam.pos);
	vec_normalize(&pos);
	ogpos = pos;

	tmp.y = 0;
	vec_mult(&tmp, perlin);
	vec_add(&pos, pos, tmp);
	// pos.y -= res / 5;
	// pos.y += 1;

	t_ray ray;
	ray.pos = level->cam.pos;
	ray.dir = pos;

	vec_normalize(&ray.dir);
	float dist = cast_face(level->all.tris[res.face_index], ray, &res);
	pos = ogpos;
	vec_mult(&pos, dist);
	vec_add(&pos, ray.pos, pos);

	// perlin = perlin_noise_2D(fabs(pos.x) + time, fabs(pos.z) + time);
	// perlin = (perlin + 1) / 2;
	// return (crossfade(0, 0x0000ff, perlin * 0xff, 0xff));

	if (fmod(fabs(pos.x), 4) > 2 ^ fmod(fabs(pos.z), 4) > 2)
		return (0xffffffff);
	return (0);
}
