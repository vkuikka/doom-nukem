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

void		update_camera(t_level *l)
{
	l->cam->lon = -l->cam->look_side + M_PI / 2;
	l->cam->lat = -l->cam->look_up;
	rot_cam(&l->cam->front, l->cam->lon, l->cam->lat);
	rot_cam(&l->cam->up, l->cam->lon, l->cam->lat + (M_PI / 2));
	vec_cross(&l->cam->side, l->cam->up, l->cam->front);
	l->cam->fov_y = l->ui->fov;
	l->cam->fov_x = l->ui->fov * ((float)RES_X / RES_Y);
}

void		render(t_window *window, t_level *level)
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
		while (i < THREAD_AMOUNT)
		{
			if (!(thread_data[i] = (t_rthread*)malloc(sizeof(t_rthread))))
				ft_error("memory allocation failed\n");
			thread_data[i]->id = i;
			thread_data[i]->level = level;
			thread_data[i]->window = window;
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
		fill_pixels(window->frame_buffer, level->ui->raycast_quality,
					level->ui->blur, level->ui->smooth_pixels);
	}
	if (level->ui->wireframe)
		wireframe(window, level);

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	ui_render(level);
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

static void		read_text_input(t_level *level, SDL_Event event)
{
	int	i;
	int	k;

	i = 0;
	if (event.type == SDL_TEXTINPUT)
	{
		while (i < SDL_TEXTINPUTEVENT_TEXT_SIZE)
		{
			char c = event.text.text[i];
			if(c < ' ' || c > '~')
				break;
			k = 0;
			while (level->ui->state.save_filename[k])
				k++;
			level->ui->state.save_filename[k] = c;
			k++;
			level->ui->state.save_filename[k] = '\0';
			i++;
		}
	}
	else if (event.type == SDL_KEYDOWN)
		if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
		{
			while (level->ui->state.save_filename[i])
				i++;
			if (!i)
				level->ui->state.text_input_enable = FALSE;
			else
			{
				i--;
				level->ui->state.save_filename[i] = '\0';
			}
		}
}

static void		read_input(t_window *window, t_camera *cam, t_editor_ui *ui, t_level *level)
{
	SDL_Event	event;
	static int	relmouse = 0;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			exit(0);
		else if (event.type == SDL_MOUSEMOTION && relmouse)
		{
			cam->look_side += (float)event.motion.xrel / 600;
			cam->look_up -= (float)event.motion.yrel / 600;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && !relmouse && ui->wireframe &&
				(event.button.x > ui->state.ui_max_width ||
				event.button.y > ui->state.ui_text_y_pos))
			select_face(level->cam, level, event.button.x, event.button.y);
		else if (event.type == SDL_MOUSEBUTTONDOWN)
			ui->state.text_input_enable = FALSE;
		else if (ui->state.text_input_enable)
			read_text_input(level, event);
		else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD)
				ui->raycast_quality += 1;
			else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA && level->ui->raycast_quality > 1)
				ui->raycast_quality -= 1;
			else if (event.key.keysym.scancode == SDL_SCANCODE_CAPSLOCK)
				ui->noclip = ui->noclip ? FALSE : TRUE;
			else if (event.key.keysym.scancode == SDL_SCANCODE_Z)
				ui->wireframe = ui->wireframe ? FALSE : TRUE;
			else if (event.key.keysym.scancode == SDL_SCANCODE_X)
				ui->show_quads = ui->show_quads ? FALSE : TRUE;
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

	level = init_level();
	init_window(&window);
	init_ui(window, level);
	init_screen_space_partition(level);
	init_physics(level);
	while (1)
	{
		frametime = SDL_GetTicks();
		read_input(window, level->cam, level->ui, level);
		physics_sync(level, NULL);
		update_camera(level);
		enemies_update_sprites(level);
		culling(level);
		screen_space_partition(level);
		render(window, level);
		level->ui->frametime = SDL_GetTicks() - frametime;
	}
}
