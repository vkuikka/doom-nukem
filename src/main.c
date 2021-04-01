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

void		split_obj(t_level *level)
{
	int		right_amount = 0;
	int		left_amount = 0;
	t_ray	right;
	t_ray	left;

	global_seginfo = "split_obj\n";
	left.pos.x = level->pos.x;
	left.pos.y = level->pos.y;
	left.pos.z = level->pos.z;
	vec_rot(&left.dir, (t_vec3){0,0,1}, level->look_side - (M_PI / 2));
	right.pos.x = level->pos.x;
	right.pos.y = level->pos.y;
	right.pos.z = level->pos.z;
	vec_rot(&right.dir, (t_vec3){0,0,1}, level->look_side + (M_PI / 2));
	for (int i = 0; i < level->visible.tri_amount; i++)
	{
		if (level->visible.tris[i].isgrid)
		{
			level->ssp[0].tris[left_amount] = level->visible.tris[i];
			level->ssp[1].tris[right_amount] = level->visible.tris[i];
			left_amount++;
			right_amount++;
		}
		else if (is_tri_side(level->visible.tris[i], left))
		{
			level->ssp[0].tris[left_amount] = level->visible.tris[i];
			left_amount++;
		}
		else if (is_tri_side(level->visible.tris[i], right))
		{
			level->ssp[1].tris[right_amount] = level->visible.tris[i];
			right_amount++;
		}
		else
		{
			level->ssp[0].tris[left_amount] = level->visible.tris[i];
			level->ssp[1].tris[right_amount] = level->visible.tris[i];
			left_amount++;
			right_amount++;
		}
	}
	level->ssp[0].tri_amount = left_amount;
	level->ssp[1].tri_amount = right_amount;
}

void		render(t_window *window, t_level *level, t_bmp *bmp)
{
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		**thread_data;
	int				window_horizontal_size;
	int				i;

	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	if (!level->ui->wireframe || (level->ui->wireframe && level->ui->wireframe_on_top))
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
		fill_pixels(window->frame_buffer, level->ui->raycast_quality);
	}
	if (level->ui->wireframe)
	{
		global_seginfo = "wireframe\n";
		wireframe(window, level);
	}

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	ui_render(level);
	SDL_RenderPresent(window->SDLrenderer);
	return ;
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
		else if (event.type == SDL_MOUSEBUTTONDOWN && !relmouse && level->ui->wireframe &&
				(event.button.x > level->ui->state.ui_max_width ||
				event.button.y > level->ui->state.ui_text_y_pos))
			select_face(level, event.button.x, event.button.y);
		else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD)
				level->ui->raycast_quality += 1;
			else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA && level->ui->raycast_quality > 1)
				level->ui->raycast_quality -= 1;
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

#if __APPLE__
	struct sigaction act;
	act.sa_handler = segv_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGSEGV, &act, NULL);
#endif

	bmp = bmp_read("out.bmp");
	level = init_level();
	init_window(&window);
	init_ui(window, level);
	init_physics(level);
	while (1)
	{
		frametime = SDL_GetTicks();
		read_input(window, level);
		physics_sync(level, NULL);
		enemies_update_sprites(level);
		culling(level);
		split_obj(level);
		render(window, level, &bmp);
		level->ui->frametime = SDL_GetTicks() - frametime;
	}
}
