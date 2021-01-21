/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/21 18:02:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

float	cast_all(t_ray vec, t_level *l, float *dist_u, float *dist_d)
{
	int		color;
	float	res = 1000000;

	for (int j = 0; j < l->obj[0].tri_amount; j++)
	{
		float tmp;
		tmp = rt_tri(l->obj[0].tris[j], vec, &color, NULL);
		if (dist_u != NULL)
		{
			if (tmp > 0 && tmp < *dist_d)
				*dist_d = tmp;
			else if (tmp < 0 && tmp > *dist_u)
				*dist_u = tmp;
		}
		else if (tmp > 0 && tmp < res)
			res = tmp;
	}
	return (res);
}

void	player_movement(float dir[3], float pos[3], t_level *l, const Uint8 *keys)
{
	static int	noclip = 1;
	static int	jump_delay = 0;
	float		speed = MOVE_SPEED;
	float		dist = 0;
	t_ray		r;

	if (keys == NULL)
	{
		if (noclip)
			noclip = 0;
		else
			noclip = 1;
		return;
	}
	if (noclip)
	{
		speed = NOCLIP_SPEED;
		if (keys[SDL_SCANCODE_SPACE])
			l->pos[1] -= 0.5;
		if (keys[SDL_SCANCODE_LSHIFT])
			l->pos[1] += 0.5;
	}
	else if (keys[SDL_SCANCODE_LSHIFT])
		speed /= 2;

	float rot[3];
	float rot_tmp[3];
	rot_tmp[0] = 0;
	rot_tmp[1] = 0;
	rot_tmp[2] = 1;

	r.pos[0] = pos[0];
	r.pos[1] = pos[1];
	r.pos[2] = pos[2];

	if (keys[SDL_SCANCODE_W])
	{
		vec_rot(rot, rot_tmp, l->look_side);
		r.dir[0] = rot[0];
		r.dir[1] = rot[1];
		r.dir[2] = rot[2];
		dist = cast_all(r, l, NULL, NULL);
		if (noclip || (dist <= 0 || dist > WALL_CLIP_DIST))
		{
			l->pos[0] += rot[0] * speed;
			l->pos[2] += rot[2] * speed;
		}
	}
	if (keys[SDL_SCANCODE_S])
	{
		vec_rot(rot, rot_tmp, l->look_side + M_PI);
		r.dir[0] = rot[0];
		r.dir[1] = rot[1];
		r.dir[2] = rot[2];
		dist = cast_all(r, l, NULL, NULL);
		if (noclip || (dist <= 0 || dist > WALL_CLIP_DIST))
		{
			l->pos[0] += rot[0] * speed;
			l->pos[2] += rot[2] * speed;
		}
	}
	if (keys[SDL_SCANCODE_D])
	{
		vec_rot(rot, rot_tmp, l->look_side + M_PI / 2);
		r.dir[0] = rot[0];
		r.dir[1] = rot[1];
		r.dir[2] = rot[2];
		dist = cast_all(r, l, NULL, NULL);
		if (noclip || (dist <= 0 || dist > WALL_CLIP_DIST))
		{
			l->pos[0] += rot[0] * speed;
			l->pos[2] += rot[2] * speed;
		}
	}
	if (keys[SDL_SCANCODE_A])
	{
		vec_rot(rot, rot_tmp, l->look_side - M_PI / 2);
		r.dir[0] = rot[0];
		r.dir[1] = rot[1];
		r.dir[2] = rot[2];
		dist = cast_all(r, l, NULL, NULL);
		if (noclip || (dist <= 0 || dist > WALL_CLIP_DIST))
		{
			l->pos[0] += rot[0] * speed;
			l->pos[2] += rot[2] * speed;
		}
	}
	if (noclip)
		return;
	if (keys[SDL_SCANCODE_SPACE] && dir[1] >= 0 && SDL_GetTicks() > jump_delay + 500)
	{
		dir[1] = -0.4;
		jump_delay = SDL_GetTicks();
	}
	float dist_d = 100000;
	float dist_u = -100000;

	r.dir[0] = 0;
	r.dir[1] = 1;
	r.dir[2] = 0;
	cast_all(r, l, &dist_u, &dist_d);
	if (dist_d > 0 && dist_d != 100000)
		dist = dist_d;
	else if (dist_u < 0 && dist_u != -100000)
		dist = dist_u;
	if (dist > 0)
	{
		if (dir[1] < 0)
			dir[1] += 0.08;
		if (dist < dir[1])
			dir[1] = 0;
		if (dist > 1 && dir[1] < 1.5)
			dir[1] += 0.04;
		else if (dist < 1)
			pos[1] += dist - 1;
	}
	else if (dist < 0 && dist > -1)
	{
		dir[1] = 0;
		pos[1] += dist - 0.5;
	}
	pos[1] += dir[1];
}

void	action_loop(t_window *window, t_level *l, t_bmp *bmp, t_obj *culled)
{
	const Uint8		*keys = SDL_GetKeyboardState(NULL);
	static float	fall_vector[3] = {0, 0, 0};
	pthread_t		threads[THREAD_AMOUNT];
	t_rthread		**thread_data;
	int				window_horizontal_size;
	int				i;

	player_movement(fall_vector, l->pos, l, keys);
	l->obj = culled;

	if (keys[SDL_SCANCODE_RIGHT])
		l->look_side += 0.1;
	if (keys[SDL_SCANCODE_LEFT])
		l->look_side -= 0.1;
	if (keys[SDL_SCANCODE_UP])
		l->look_up += 0.1;
	if (keys[SDL_SCANCODE_DOWN])
		l->look_up -= 0.1;

	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	i = 0;
	if (!(thread_data = (t_rthread**)malloc(sizeof(t_rthread*) * THREAD_AMOUNT)))
		ft_error("memory allocation failed\n");
	while (i < THREAD_AMOUNT)
	{
		if (!(thread_data[i] = (t_rthread*)malloc(sizeof(t_rthread))))
			ft_error("memory allocation failed\n");
		thread_data[i]->id = i;
		thread_data[i]->level = l;
		thread_data[i]->window = window;
		thread_data[i]->img = bmp;
		pthread_create(&threads[i], NULL, rt_test, (void*)thread_data[i]);
		i++;
	}
	i = 0;
	while (i < THREAD_AMOUNT)
	{
		pthread_join(threads[i], NULL);
		free(thread_data[i]);
		i++;
	}
	free(thread_data);
	fill_pixels(window->frame_buffer, PIXEL_GAP);

	/////////////////////bmp
	// for (int y = 0; y < bmp->height; y++)
	// {
	// 	for (int x = 0; x < bmp->width; x++)
	// 	{
	// 		window->frame_buffer[(y * (int)RES_X) + x] = bmp->image[y * bmp->width + x];
	// 	}
	// }
	/////////////////////bmp

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

int		get_fps(void)
{
	struct timeval	time;
	static long		s = 0;
	static int		i = 0;
	static int		fps = 0;

	i++;
	gettimeofday(&time, NULL);
	if (s != time.tv_sec)
	{
		s = time.tv_sec;
		fps = i;
		i = 0;
	}
	return fps;
}

int			main(int argc, char **argv)
{
	SDL_Event	event;
	t_window	*window;
	t_level		*level;
	unsigned	frametime;
	t_bmp		bmp;
	int			enable_culling;

	enable_culling = 1;
	bmp = bmp_read("out.bmp");
	level = rt_test_init_level();
	init_window(&window);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (1)
	{
		frametime = SDL_GetTicks();
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				return (0);
			else if (event.type == SDL_MOUSEMOTION)
			{
				level->look_side += (float)event.motion.xrel / 200;
				level->look_up -= (float)event.motion.yrel / 200;
			}
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_N)
				player_movement((float[3]){0,0,0}, (float[3]){0,0,0}, NULL, NULL);
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_M)
				enable_culling = enable_culling ? 0 : 1;
		}
		int faces = level->obj->tri_amount;
		t_obj *tmp = level->obj;
		t_obj *culled = level->obj;
		if (enable_culling)
		{
			faces = 0;
			culled = culling(level, &faces);
		}
		action_loop(window, level, &bmp, culled);
		if (enable_culling)
		{
			free(culled->tris);
			free(culled);
			level->obj = tmp;
		}

		frametime = SDL_GetTicks() - frametime;
		//printf("time: %d ms\n", frametime);
		char buf[50];
		int fps = get_fps();
		sprintf(buf, "%dfps %dms %dfaces\n", fps, frametime, faces);
		SDL_SetWindowTitle(window->SDLwindow, buf);

		if (frametime < 100)
			usleep(10000);
	}
}
