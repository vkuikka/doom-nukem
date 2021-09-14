/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/10 22:39:12 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/02 19:17:44 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	typing_input_backspace(t_level *level)
{
	int	i;

	i = 0;
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

static void	typing_input(t_level *level, SDL_Event event)
{
	int		i;
	int		k;
	char	c;

	if (event.type == SDL_TEXTINPUT)
	{
		i = 0;
		while (i < SDL_TEXTINPUTEVENT_TEXT_SIZE)
		{
			c = event.text.text[i];
			if (c < ' ' || c > '~')
				break ;
			k = 0;
			while (level->ui.state.save_filename[k])
				k++;
			level->ui.state.save_filename[k] = c;
			k++;
			level->ui.state.save_filename[k] = '\0';
			i++;
		}
	}
	else if (event.type == SDL_KEYDOWN
		&& event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
		typing_input_backspace(level);
}

static void	set_mouse_input_location_3d_space(t_level *level, int x, int y)
{
	gizmo(level);
	if (level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_X
		&& level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_Y
		&& level->ui.state.mouse_location != MOUSE_LOCATION_GIZMO_Z)
	{
		if (level->ui.state.ui_location == UI_LOCATION_LIGHT_EDITOR)
		{
			level->ui.state.mouse_location = MOUSE_LOCATION_LIGHT_EDITOR;
			if (level->ui.state.m1_click && level->bake_status != BAKE_BAKING)
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

static void	set_mouse_location(t_level *level, int *x, int *y)
{
	if (level->ui.state.mouse_capture)
	{
		*x = RES_X / 2;
		*y = RES_Y / 2;
	}
	else
	{
		SDL_GetMouseState(x, y);
		level->ui.state.mouse.x = *x;
		level->ui.state.mouse.y = *y;
	}
}

static void	set_mouse_input_location(t_level *level, t_game_state game_state)
{
	int	x;
	int	y;

	set_mouse_location(level, &x, &y);
	if (game_state == GAME_STATE_MAIN_MENU)
	{
		if (level->ui.main_menu == MAIN_MENU_LOCATION_MAIN)
			level->ui.state.mouse_location = MOUSE_LOCATION_MAIN_MENU;
		else
			level->ui.state.mouse_location = MOUSE_LOCATION_UI;
	}
	else if (level->ui.state.mouse_capture || game_state == GAME_STATE_INGAME)
		level->ui.state.mouse_location = MOUSE_LOCATION_GAME;
	else if (x < level->ui.state.ui_max_width
		&& y < level->ui.state.ui_text_y_pos)
		level->ui.state.mouse_location = MOUSE_LOCATION_UI;
	else if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR
		&& x < RES_X / 2)
		level->ui.state.mouse_location = MOUSE_LOCATION_UV_EDITOR;
	else
		set_mouse_input_location_3d_space(level, x, y);
	gizmo(level);
	level->ui.state.m1_click = FALSE;
}

static void	mouse_input(t_level *level, SDL_Event event,
										t_game_state game_state)
{
	if (event.type == SDL_MOUSEMOTION && level->ui.state.mouse_capture
		&& game_state != GAME_STATE_DEAD)
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

static void	toggle_mouse_capture(t_level *level, t_window *window,
				t_game_state *game_state)
{
	if (*game_state == GAME_STATE_MAIN_MENU)
		return ;
	level->ui.state.mouse_capture = level->ui.state.mouse_capture == 0;
	if (level->ui.state.mouse_capture)
		SDL_SetRelativeMouseMode(SDL_TRUE);
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_WarpMouseInWindow(window->SDLwindow, RES_X / 2, RES_Y / 2);
	}
}

static void	ui_go_back(t_level *level, t_game_state *game_state)
{
	if (level->bake_status == BAKE_BAKING)
	{
		nonfatal_error("cancel baking first (press baking button)");
		return ;
	}
	level->ui.main_menu = MAIN_MENU_LOCATION_MAIN;
	if (*game_state != GAME_STATE_MAIN_MENU)
	{
		level->ui.state.mouse_capture = FALSE;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		*game_state = GAME_STATE_MAIN_MENU;
		level->main_menu_anim_start_time = SDL_GetTicks();
		level->ui.state.ui_location = UI_LOCATION_MAIN;
		level->win_start_time = 0;
		level->death_start_time = 0;
		Mix_PlayMusic(level->audio.title_music, -1);
	}
}

static void	keyboard_input(t_window *window, t_level *level, SDL_Event event,
													t_game_state *game_state)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD)
		level->ui.raycast_quality += 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA
		&& level->ui.raycast_quality > 1)
		level->ui.raycast_quality -= 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_CAPSLOCK
		&& *game_state != GAME_STATE_INGAME)
		level->ui.noclip = level->ui.noclip == 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_Z)
		level->ui.wireframe = level->ui.wireframe == 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_X)
		level->ui.show_quads = level->ui.show_quads == 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_TAB)
		toggle_mouse_capture(level, window, game_state);
	else if (event.key.keysym.scancode == SDL_SCANCODE_O
		&& *game_state == GAME_STATE_EDITOR)
		toggle_selection_all(level);
	else if (event.key.keysym.scancode == SDL_SCANCODE_E)
		door_activate(level);
	else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
		ui_go_back(level, game_state);
	else if (event.key.keysym.scancode == SDL_SCANCODE_R)
		level->reload_start_time = SDL_GetTicks();
	else if (event.key.keysym.scancode == SDL_SCANCODE_T)
		spray(level->cam, level);
}

void	read_input(t_window *window, t_level *level,
									t_game_state *game_state)
{
	SDL_Event	event;

	set_mouse_input_location(level, *game_state);
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT
			|| event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			exit(0);
		mouse_input(level, event, *game_state);
		if (level->ui.state.text_input_enable)
			typing_input(level, event);
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
			keyboard_input(window, level, event, game_state);
	}
}
