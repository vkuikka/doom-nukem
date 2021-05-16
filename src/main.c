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
	l->cam.lon = -l->cam.look_side + M_PI / 2;
	l->cam.lat = -l->cam.look_up;
	rot_cam(&l->cam.front, l->cam.lon, l->cam.lat);
	rot_cam(&l->cam.up, l->cam.lon, l->cam.lat + (M_PI / 2));
	vec_cross(&l->cam.side, l->cam.up, l->cam.front);
	l->cam.fov_y = l->ui.fov;
	l->cam.fov_x = l->ui.fov * ((float)RES_X / RES_Y);
}

void		render(t_window *window, t_level *level)
{
	SDL_Thread		*threads[THREAD_AMOUNT];
	t_rthread		**thread_data;
	int				window_horizontal_size;
	int				i;

	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	if (!level->ui.wireframe || (level->ui.wireframe && level->ui.wireframe_on_top))
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
		fill_pixels(window->frame_buffer, level->ui.raycast_quality,
					level->ui.blur, level->ui.smooth_pixels);
	}
	wireframe(window, level);

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	obj_editor(level, window);
	if (level->ui.state.is_uv_editor_open)
		uv_editor(level, window);
	ui_render(level);
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

static void		typing_input(t_level *level, SDL_Event event)
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
			while (level->ui.state.save_filename[k])
				k++;
			level->ui.state.save_filename[k] = c;
			k++;
			level->ui.state.save_filename[k] = '\0';
			i++;
		}
	}
	else if (event.type == SDL_KEYDOWN)
		if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
		{
			while (level->ui.state.save_filename[i])
				i++;
			if (!i)
				level->ui.state.text_input_enable = FALSE;
			else
			{
				i--;
				level->ui.state.save_filename[i] = '\0';
			}
		}
}

static void		set_mouse_input_location(t_level *level)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	if (level->ui.state.mouse_capture)
		level->ui.state.mouse_location = MOUSE_LOCATION_GAME;
	else if (x < level->ui.state.ui_max_width && y < level->ui.state.ui_text_y_pos)
		level->ui.state.mouse_location = MOUSE_LOCATION_UI;
	else if (level->ui.state.is_uv_editor_open && x < RES_X / 2)
		level->ui.state.mouse_location = MOUSE_LOCATION_UV_EDITOR;
	else
	{
		obj_editor_input(level);
		if (level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_X &&
		level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_Y &&
		level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_Z)
		{
			level->ui.state.mouse_location = MOUSE_LOCATION_SELECTION;
			level->ui.state.m1_drag = FALSE;
			select_face(&level->cam, level, x, y);
		}
	}
	level->ui.state.m1_click = FALSE;
}

static void		mouse_input(t_level *level, SDL_Event event)
{
	if (event.type == SDL_MOUSEMOTION && level->ui.state.mouse_capture)
	{
		level->cam.look_side += (float)event.motion.xrel / 600;
		level->cam.look_up -= (float)event.motion.yrel / 600;
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
		level->ui.state.m1_drag = FALSE;
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		level->ui.state.text_input_enable = FALSE;
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			level->ui.state.m1_click = TRUE;
			level->ui.state.m1_drag = TRUE;
		}
	}
}

static void		keyboard_input(t_window *window, t_level *level, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD)
		level->ui.raycast_quality += 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA && level->ui.raycast_quality > 1)
		level->ui.raycast_quality -= 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_CAPSLOCK)
		level->ui.noclip = level->ui.noclip ? FALSE : TRUE;
	else if (event.key.keysym.scancode == SDL_SCANCODE_Z)
		level->ui.wireframe = level->ui.wireframe ? FALSE : TRUE;
	else if (event.key.keysym.scancode == SDL_SCANCODE_X)
		level->ui.show_quads = level->ui.show_quads ? FALSE : TRUE;
	else if (event.key.keysym.scancode == SDL_SCANCODE_TAB)
	{
		level->ui.state.mouse_capture = level->ui.state.mouse_capture ? FALSE : TRUE;
		if (level->ui.state.mouse_capture)
			SDL_SetRelativeMouseMode(SDL_TRUE);
		else
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_WarpMouseInWindow(window->SDLwindow, RES_X / 2, RES_Y / 2);
		}
	}
	else if (event.key.keysym.scancode == SDL_SCANCODE_O)
		toggle_selection_all(level);
}

static void		read_input(t_window *window, t_level *level)
{
	SDL_Event	event;

	set_mouse_input_location(level);
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			exit(0);
		mouse_input(level, event);
		if (level->ui.state.text_input_enable)
			typing_input(level, event);
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
			keyboard_input(window, level, event);
	}
}

int			main(int argc, char **argv)
{
	t_window	*window;
	t_level		*level;
	unsigned	ssp;
	unsigned	cull;
	unsigned	rendertime;
	unsigned	frametime;

	init_level(&level);
	init_window(&window);
	init_ui(window, level);
	init_screen_space_partition(level);
	init_culling(level);
	while (1)
	{
		frametime = SDL_GetTicks();
		read_input(window, level);
		player_movement(level);
		update_camera(level);
		if (!level->ui.state.is_uv_editor_open)
			enemies_update_sprites(level);
		cull = SDL_GetTicks();
		culling(level);
		level->ui.cull = SDL_GetTicks() - cull;
		ssp = SDL_GetTicks();
		screen_space_partition(level);
		level->ui.ssp = SDL_GetTicks() - ssp;
		rendertime = SDL_GetTicks();
		render(window, level);
		level->ui.render = SDL_GetTicks() - rendertime;
		level->ui.frametime = SDL_GetTicks() - frametime;
	}
}
