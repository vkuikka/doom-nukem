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

void		render(t_window *window, t_level *level, t_game_state *game_state)
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
	if (*game_state == GAME_STATE_EDITOR && level->ui.state.ui_location != UI_LOCATION_SETTINGS)
		wireframe(window, level);
	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	if (*game_state == GAME_STATE_INGAME || *game_state == GAME_STATE_DEAD || *game_state == GAME_STATE_WIN)
		hud(level, window, *game_state);
	else if (*game_state == GAME_STATE_MAIN_MENU)
		main_menu(level, window, game_state);
	else
	{
		gizmo_render(level, window);
		if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
			uv_editor(level, window);
		ui_render(level);
	}
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

static void		set_mouse_input_location(t_level *level, t_game_state game_state)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	if (game_state == GAME_STATE_MAIN_MENU)
		level->ui.state.mouse_location = MOUSE_LOCATION_MAIN_MENU;
	else if (level->ui.state.mouse_capture || game_state == GAME_STATE_INGAME)
		level->ui.state.mouse_location = MOUSE_LOCATION_GAME;
	else if (x < level->ui.state.ui_max_width && y < level->ui.state.ui_text_y_pos)
		level->ui.state.mouse_location = MOUSE_LOCATION_UI;
	else if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR && x < RES_X / 2)
		level->ui.state.mouse_location = MOUSE_LOCATION_UV_EDITOR;
	else
	{
		gizmo(level);
		if (level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_X &&
		level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_Y &&
		level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_Z)
		{
			if (level->ui.state.ui_location == UI_LOCATION_LIGHT_EDITOR)
			{
				level->ui.state.mouse_location = MOUSE_LOCATION_LIGHT_EDITOR;
				if (level->ui.state.m1_click)
					select_light(level, x, y);
			}
			else
			{
				level->ui.state.mouse_location = MOUSE_LOCATION_SELECTION;
				select_face(&level->cam, level, x, y);
			}
			level->ui.state.m1_drag = FALSE;
		}
	}
	gizmo(level);
	level->ui.state.m1_click = FALSE;
}

static void		mouse_input(t_level *level, SDL_Event event, t_game_state game_state)
{
	if (event.type == SDL_MOUSEMOTION && level->ui.state.mouse_capture && game_state != GAME_STATE_DEAD)
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

static void		keyboard_input(t_window *window, t_level *level, SDL_Event event, t_game_state *game_state)
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
	else if (event.key.keysym.scancode == SDL_SCANCODE_TAB && *game_state != GAME_STATE_MAIN_MENU)
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
	else if (event.key.keysym.scancode == SDL_SCANCODE_O && *game_state == GAME_STATE_EDITOR)
		toggle_selection_all(level);
	else if (event.key.keysym.scancode == SDL_SCANCODE_E)
		door_activate(level);
	else if (event.key.keysym.scancode == SDL_SCANCODE_Q && *game_state != GAME_STATE_MAIN_MENU)
	{
		level->ui.state.mouse_capture = FALSE;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		*game_state = GAME_STATE_MAIN_MENU;
		level->ui.state.ui_location = UI_LOCATION_MAIN;
		level->cam.pos = level->main_menu_pos1.pos;
		level->cam.look_side = level->main_menu_pos1.look_side;
		level->cam.look_up = level->main_menu_pos1.look_up;
		level->main_menu_anim_start_time = SDL_GetTicks();
		level->win_start_time = 0;
		level->death_start_time = 0;
		Mix_PlayMusic(level->audio.music, -1);
	}
	else if (event.key.keysym.scancode == SDL_SCANCODE_R)
	{
		Mix_PlayChannel(AUDIO_RELOAD_CHANNEL, level->audio.reload, 0);
		level->reload_start_time = SDL_GetTicks();
	}
	else if (event.key.keysym.scancode == SDL_SCANCODE_T)
		spray(level->cam, level);
	else if (event.key.keysym.scancode == SDL_SCANCODE_Y)
		ft_bzero(level->spray_overlay, level->texture.width * level->texture.height * 4);
}

static void		read_input(t_window *window, t_level *level, t_game_state *game_state)
{
	SDL_Event	event;

	set_mouse_input_location(level, *game_state);
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			exit(0);
		mouse_input(level, event, *game_state);
		if (level->ui.state.text_input_enable)
			typing_input(level, event);
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
			keyboard_input(window, level, event, game_state);
	}
}

static void	player_reload(t_level *level)
{
	float time = (SDL_GetTicks() - level->reload_start_time) / (1000 * VIEWMODEL_ANIM_FPS);
	if (time < 1)
		level->viewmodel_index = (int)(time * VIEWMODEL_FRAMES);
	else
	{
		level->viewmodel_index = 0;
		level->reload_start_time = 0;
		level->player_ammo = PLAYER_AMMO_MAX;
	}
}

void		game_logic(t_level *level, t_game_state *game_state)
{
	if (level->reload_start_time && *game_state != GAME_STATE_DEAD)
		player_reload(level);
	if (level->ui.state.m1_click && level->ui.state.mouse_capture && *game_state != GAME_STATE_DEAD)
	{
		if (level->player_ammo)
		{
			Mix_PlayChannel(AUDIO_GUNSHOT_CHANNEL, level->audio.gunshot, 0);
			level->player_ammo--;
			level->player.dir = level->cam.front;
			create_projectile(level, level->cam.pos, level->cam.front, &level->player);
		}
		else
		{
			Mix_PlayChannel(AUDIO_RELOAD_CHANNEL, level->audio.reload, 0);
			level->reload_start_time = SDL_GetTicks();
		}
	}
	if (level->win_start_time)
	{
		float time = (SDL_GetTicks() - level->win_start_time) / (1000.0 * WIN_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->win_start_time = 0;
		}
	}
	else if (level->death_start_time)
	{
		level->player_health = 0;
		float time = (SDL_GetTicks() - level->death_start_time) / (1000.0 * DEATH_LENGTH_SEC);
		if (time > 1)
		{
			*game_state = GAME_STATE_MAIN_MENU;
			level->ui.state.mouse_capture = FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			level->death_start_time = 0;
		}
	}
	else if (level->player_health <= 0)
	{
		Mix_HaltMusic();
		Mix_PlayChannel(AUDIO_DEATH_CHANNEL, level->audio.death, 0);
		level->reload_start_time = 0;
		level->death_start_time = SDL_GetTicks();
		*game_state = GAME_STATE_DEAD;
		level->cam.look_up = 1;
	}
	else if (*game_state == GAME_STATE_INGAME)
	{
		t_vec3 dist;
		vec_sub(&dist, level->win_pos, level->cam.pos);
		if (vec_length(dist) < level->win_dist)
		{
			*game_state = GAME_STATE_WIN;
			level->win_start_time = SDL_GetTicks();
		}
	}
}

int			main(int argc, char **argv)
{
	t_window	*window;
	t_level		*level;
	t_game_state game_state;
	unsigned	ssp;
	unsigned	cull;
	unsigned	rendertime;
	unsigned	frametime;

	game_state = GAME_STATE_MAIN_MENU;
	game_state = GAME_STATE_EDITOR;//remove
	level = init_level();
	init_audio(level);
	init_window(&window);
	init_ui(window, level);
	init_screen_space_partition(level);
	init_culling(level);
	init_player(&level->player);

	open_level(level, "level/demo.doom-nukem");
	start_bake(level);
	while (1)
	{
		frametime = SDL_GetTicks();
		read_input(window, level, &game_state);
		if (game_state == GAME_STATE_MAIN_MENU)
			main_menu_move_background(level);
		else
		{
			if (game_state == GAME_STATE_INGAME || game_state == GAME_STATE_DEAD || game_state == GAME_STATE_WIN)
				game_logic(level, &game_state);
			if (game_state != GAME_STATE_DEAD)
				player_movement(level, game_state);
		}
		update_camera(level);
		door_animate(level);
		if (game_state == GAME_STATE_EDITOR)
		{
			door_put_text(window, level);
			light_put_text(window, level);
		}
		enemies_update_sprites(level);
		cull = SDL_GetTicks();
		culling(level);
		level->ui.cull = SDL_GetTicks() - cull;
		ssp = SDL_GetTicks();
		screen_space_partition(level);
		level->ui.ssp = SDL_GetTicks() - ssp;
		rendertime = SDL_GetTicks();
		render(window, level, &game_state);
		level->ui.render = SDL_GetTicks() - rendertime;
		level->ui.frametime = SDL_GetTicks() - frametime;
	}
}
