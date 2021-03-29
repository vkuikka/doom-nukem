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

void        segv_handler(int sig)
{
	printf ("\n\033[1m\033[31m\tSEGFAULT: %s\n\033[0m", global_seginfo);
	abort();
	(void)sig;
}

int			is_tri_side(t_tri tri, t_ray c)
{
	t_vec3   end;

	int amount = tri.isquad ? 4 : 3;
	for (int i = 0; i < amount; i++)
	{
		vec_sub(&end, tri.verts[i].pos, c.pos);
		if (vec_dot(end, c.dir) <= 0)
			return (FALSE);
	}
	return (TRUE);
}

void		split_obj(t_obj *culled, t_level *level, int *faces_left, int *faces_right)
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

void	    render(t_window *window, t_level *level, t_bmp *bmp, t_obj *culled, int *faces_left, int *faces_right)
{
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		**thread_data;
	int				window_horizontal_size;
	int				i;

	if (!level->ui->wireframe)
	{
		global_seginfo = "split_obj\n";
		split_obj(culled, level, faces_left, faces_right);
		level->obj = culled;
	}
	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	if (level->ui->wireframe)
	{
		global_seginfo = "wireframe\n";
		wireframe(window, level);
	}
	else
	{
		i = 0;
		if (!(thread_data = (t_rthread**)malloc(sizeof(t_rthread*) * THREAD_AMOUNT)))
			ft_error("memory allocation failed\n");
		global_seginfo = "raycast\n";
		while (i < THREAD_AMOUNT)
		{
			if (!(thread_data[i] = (t_rthread*)malloc(sizeof(t_rthread))))
				ft_error("memory allocation failed\n");
			thread_data[i]->id = i;
			thread_data[i]->level = level;
			thread_data[i]->window = window;
			thread_data[i]->img = bmp;
			threads[i] = SDL_CreateThread(raycast, "asd", (void*)thread_data[i]);
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
		fill_pixels(window->frame_buffer, level->quality);
	}

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	draw_buttons();
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

int		    get_fps(int i)
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

static void		read_input(t_window *window, t_level *level)
{
	SDL_Event	event;
	static int	relmouse = 0;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			exit(0);
		else if (event.type == SDL_MOUSEMOTION && relmouse)
		{
			level->look_side += (float)event.motion.xrel / 600;
			level->look_up -= (float)event.motion.yrel / 600;
		}
		else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD)
				level->quality += 1;
			else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA && level->quality > 1)
				level->quality -= 1;
			else if (event.key.keysym.scancode == SDL_SCANCODE_CAPSLOCK)
				level->ui->noclip = level->ui->noclip ? FALSE : TRUE;
			else if (event.key.keysym.scancode == SDL_SCANCODE_Z)
				level->ui->wireframe = level->ui->wireframe ? FALSE : TRUE;
			else if (event.key.keysym.scancode == SDL_SCANCODE_X)
				level->ui->show_quads = level->ui->show_quads ? FALSE : TRUE;
			else if (event.key.keysym.scancode == SDL_SCANCODE_TAB)
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
	}
}

int			main(int argc, char **argv)
{
	t_window	*window;
	t_editor_ui	ui;
	t_level		*level;
	unsigned	frametime;
	t_bmp		bmp;
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

	global_seginfo = "bmp_read\n";
	bmp = bmp_read("out.bmp");
	level = init_level();
	init_window(&window);
	init_buttons(window, &ui);
	level->ui = &ui;
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
	pos = level->pos;
	SDL_CreateThread(physics, "physics", (void*)&physicsdata);
	while (1)
	{
		frametime = SDL_GetTicks();
		read_input(window, level);
		level->pos = pos;
		enemies(level);
		t_obj *tmp = level->obj;
		int faces_visible = level->obj->tri_amount;
		int faces_left = culled[0].tri_amount;
		int faces_right = culled[0].tri_amount;
		if (!level->ui->wireframe)
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
		render(window, level, &bmp, culled, &faces_left, &faces_right);
		level->obj = tmp;

		frametime = SDL_GetTicks() - frametime;
		char buf[50];
		int fps = get_fps(0);
		sprintf(buf, "%.2fhz %dfps %d(%dL %dR)faces quality: %d", physhz, fps, faces_visible, faces_left, faces_right, level->quality);
		SDL_SetWindowTitle(window->SDLwindow, buf);
	}
}
