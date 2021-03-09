/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/02/20 04:37: 0by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void segv_handler(int sig)
{
	printf ("\n\033[1m\033[31m\tSEGFAULT: %s\n\033[0m", global_seginfo);
    abort();
    (void)sig;
}

int				is_tri_side(t_tri tri, t_ray c)
{
	t_vec3   end;

	int amount = tri.isquad ? 4 : 3;
	for (int i = 0; i < amount; i++)
	{
		vec_sub(&end, tri.verts[i].pos, c.pos);
		if (vec_dot(end, c.dir) <= 0)
			return (0);
	}
	return (1);
}

void			split_obj(t_obj *culled, t_level *level, int *faces_left, int *faces_right)
{
	int		right_amount = 0;
	int		left_amount = 0;
	t_ray	right;
	t_ray	left;

	left.pos.x = level->pos.x;
	left.pos.y = level->pos.y;
	left.pos.z = level->pos.z;
	vec_rot(&left.dir, (t_vec3){0,0,1}, level->look_side - (M_PI / 2));
	right.pos.x = level->pos.x;
	right.pos.y = level->pos.y;
	right.pos.z = level->pos.z;
	vec_rot(&right.dir, (t_vec3){0,0,1}, level->look_side + (M_PI / 2));
	for (int i = 0; i < culled->tri_amount; i++)
	{
		if (culled[0].tris[i].isgrid)
		{
			culled[0].tris[left_amount] = culled[0].tris[i];
			culled[1].tris[right_amount] = culled[0].tris[i];
			left_amount++;
			right_amount++;
		}
		else if (is_tri_side(culled[0].tris[i], left))
		{
			culled[0].tris[left_amount] = culled[0].tris[i];
			left_amount++;
		}
		else if (is_tri_side(culled[0].tris[i], right))
		{
			culled[1].tris[right_amount] = culled[0].tris[i];
			right_amount++;
		}
		else
		{
			culled[0].tris[left_amount] = culled[0].tris[i];
			culled[1].tris[right_amount] = culled[0].tris[i];
			left_amount++;
			right_amount++;
		}
	}
	culled[0].tri_amount = left_amount;
	culled[1].tri_amount = right_amount;
	(*faces_left) = left_amount;
	(*faces_right) = right_amount;
}

void	action_loop(t_window *window, t_level *l, t_bmp *bmp, t_obj *culled, int *faces_left, int *faces_right, int rendermode)
{
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		**thread_data;
	int				window_horizontal_size;
	int				i;

	if (rendermode != 2)
	{
		global_seginfo = "split_obj\n";
		split_obj(culled, l, faces_left, faces_right);
		l->obj = culled;
	}
	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	if (rendermode == 2)
	{
		global_seginfo = "wireframe\n";
		wireframe(window, l);
	}
	else
	{
		i = 0;
		if (!(thread_data = (t_rthread**)malloc(sizeof(t_rthread*) * THREAD_AMOUNT)))
			ft_error("memory allocation failed\n");
		global_seginfo = "render\n";
		while (i < THREAD_AMOUNT)
		{
			if (!(thread_data[i] = (t_rthread*)malloc(sizeof(t_rthread))))
				ft_error("memory allocation failed\n");
			thread_data[i]->id = i;
			thread_data[i]->level = l;
			thread_data[i]->window = window;
			thread_data[i]->img = bmp;
			threads[i] = SDL_CreateThread(render, "asd", (void*)thread_data[i]);
			i++;
		}
		i = 0;
		while (i < THREAD_AMOUNT)
		{
			int thread_returnvalue;
			SDL_WaitThread(threads[i], &thread_returnvalue);
			free(thread_data[i]);
			i++;
		}
		free(thread_data);
		global_seginfo = "fill_pixels\n";
		fill_pixels(window->frame_buffer, l->quality);
	}

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	draw_buttons();
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

int		get_fps(int i)
{
	struct timeval	time;
	static long		s[2];
	static int		frames[2];
	static int		fps[2];

	frames[i]++;
	gettimeofday(&time, NULL);
	if (s[i] != time.tv_sec)
	{
		s[i] = time.tv_sec;
		fps[i] = frames[i];
		frames[i] = 0;
	}
	return (fps[i]);
}

int		did_move(t_level *level)
{
	static int		last_side = 0;
	static int		last_up = 0;
	static t_vec3	pos;
	int				res = 0;

	if (last_side != level->look_side && last_up != level->look_up && !vec_cmp(pos, level->pos))
		res = 1;
	last_side = level->look_side;
	last_up = level->look_up;
	pos = level->pos;
	return (res);
}

void	set_quality(int frametime, t_level *level, int currentfps)
{
	static int	lastfps = 0;
	static int	s = 0;
	const int	pause_time = 2;//seconds until can improve quality
	int			minfps = TARGETFPS - 10;
	int		maxfps = TARGETFPS + 10;

	if (lastfps != currentfps)
		s++;
	lastfps = currentfps;
	if (frametime > 1000 / minfps)
		level->quality += 2;
	else if (s == pause_time && did_move(level) && frametime < 1000 / maxfps)
		level->quality -= 2;
	did_move(level);//update statics
	if (s == pause_time)
		s = 0;
	if (level->quality < 0)
		level->quality = 1;
	else if (level->quality > 13)
		level->quality = 13;
}

int			main(int argc, char **argv)
{
	SDL_Event	event;
	t_window	*window;
	t_level		*level;
	unsigned	frametime;
	t_bmp		bmp;
	int			rendermode;//0 raycast all, 1 raycast culled, 2 wireframe
	int			relmouse;
	t_obj		*culled;
	t_physthread	physicsdata;
	t_vec3		pos;

#if __APPLE__
	struct sigaction act;
	act.sa_handler = segv_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGSEGV, &act, NULL);
#endif

	relmouse = 0;
	rendermode = 1;
	global_seginfo = "bmp_read\n";
	bmp = bmp_read("out.bmp");
	global_seginfo = "init_level\n";
	level = init_level();
	level->quality = 1;
	global_seginfo = "init_window\n";
	init_window(&window);
	init_buttons(window);
	if (!(culled = (t_obj*)malloc(sizeof(t_obj) * 2)))
		ft_error("memory allocation failed\n");
	if (!(culled[0].tris = (t_tri*)malloc(sizeof(t_tri) * level->obj->tri_amount)))
		ft_error("memory allocation failed\n");
	if (!(culled[1].tris = (t_tri*)malloc(sizeof(t_tri) * level->obj->tri_amount)))
		ft_error("memory allocation failed\n");
	for (int i = 0; i < level->obj->tri_amount; i++)
	{
		culled[0].tris[i] = level->obj[0].tris[i];
		culled[1].tris[i] = level->obj[0].tris[i];
	}
	culled[0].tri_amount = level->obj[0].tri_amount;
	culled[1].tri_amount = level->obj[0].tri_amount;
	level->allfaces = level->obj;
	physicsdata.level = level;
	float physhz = 0;
	physicsdata.hz = &physhz;
	physicsdata.pos = &pos;
	pos.x = level->pos.x;
	pos.y = level->pos.y;
	pos.z = level->pos.z;
	SDL_CreateThread(physics, "physics", (void*)&physicsdata);
	while (1)
	{
		frametime = SDL_GetTicks();
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				return (0);
			else if (event.type == SDL_MOUSEMOTION && relmouse)
			{
				level->look_side += (float)event.motion.xrel / 600;
				level->look_up -= (float)event.motion.yrel / 600;
			}
			/*else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				level->look_side += 0.1;
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				level->look_side -= 0.1;
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_UP)
				level->look_up += 0.1;
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				level->look_up -= 0.1;*/
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_N)
				player_movement(NULL, NULL);
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_M)
			{
				rendermode = rendermode == 2 ? 0 : rendermode + 1;
				level->quality = 1;
			}
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_TAB)
			{
				relmouse = relmouse ? 0 : 1;
				if (relmouse)
					SDL_SetRelativeMouseMode(SDL_TRUE);
				else
				{
					SDL_SetRelativeMouseMode(SDL_FALSE);
					SDL_WarpMouseInWindow(window->SDLwindow, RES_X / 2, RES_Y / 2);
				}
			}
		}
		level->pos = pos;
		t_obj *tmp = level->obj;
		int faces_visible = level->obj->tri_amount;
		int faces_left = culled[0].tri_amount;
		int faces_right = culled[0].tri_amount;
		if (rendermode == 1)
		{
			faces_visible = 0;
			culling(level, &faces_visible, culled);
		}
		else
		{
			for (int i = 0; i < level->obj->tri_amount; i++)
				culled[0].tris[i] = level->obj[0].tris[i];
			culled[0].tri_amount = level->obj[0].tri_amount;
		}
		button(&level->enable_fog, "fog");
		button(&rendermode, "culling");
		action_loop(window, level, &bmp, culled, &faces_left, &faces_right, rendermode);
		level->obj = tmp;

		frametime = SDL_GetTicks() - frametime;
		//printf("time: %d ms\n", frametime);
		char buf[50];
		int fps = get_fps(0);
		set_quality(frametime, level, fps);
		sprintf(buf, "%.2fhz %dfps %d(%dL %dR)faces quality: %d", physhz, fps, faces_visible, faces_left, faces_right, level->quality);
		SDL_SetWindowTitle(window->SDLwindow, buf);

		//SDL_Delay(2);
		//if (frametime < 100)
		//	usleep(10000);
	}
}
