/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_elements.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/13 12:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/14 00:36:09 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

t_window	*get_window(t_window *get_window)
{
	static t_window	*window = NULL;

	if (get_window)
		window = get_window;
	else
		return (window);
	return (NULL);

}

t_ui_state	*get_ui_state(t_ui_state *get_state)
{
	static t_ui_state	*state = NULL;

	if (get_state)
		state = get_state;
	else
		return (state);
	return (NULL);
}

static void	edit_slider_var(float *unit, t_ui_state *state)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_drag
		&& y >= state->ui_text_y_pos + 4 && y <= state->ui_text_y_pos + 15
		&& x < 109 && x > 2)
		*unit = (float)(x - 4) / (float)100;
}

static int	edit_call_var(t_ui_state *state, t_ivec2 size)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_click
		&& x >= 3 && x <= size.x + 6 && y >= state->ui_text_y_pos + 4
		&& y <= state->ui_text_y_pos + size.y + 2)
		return (TRUE);
	return (FALSE);
}

static int	edit_button_var(int *var, t_ui_state *state)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_click
		&& x >= 2 && x <= 11 && y >= state->ui_text_y_pos
		&& y <= state->ui_text_y_pos + UI_ELEMENT_HEIGHT)
	{
		*var = *var == 0;
		return (TRUE);
	}
	return (FALSE);
}

void	render_call_streaming(unsigned int *texture, int dy,
											t_ivec2 *size, int color)
{
	int					x;
	int					y;

	y = 0;
	while (y < size->y - 1)
	{
		x = 0;
		while (x < size->x + 4)
		{
			button_pixel_put(x + 2, y + 2 + dy, color, texture);
			x++;
		}
		y++;
	}
}

void	render_button_streaming(unsigned int *texture, int *var, int dy)
{
	int					color;
	int					x;
	int					y;

	color = 0x303030ff;
	if (*var)
		color = 0x008020ff;
	y = 0;
	while (y < 10)
	{
		x = 0;
		while (x < 10)
		{
			if (y < 1 || y > 8 || x < 1 || x > 8)
				button_pixel_put(x + 2, y + 4 + dy, 0x404040ff, texture);
			else
				button_pixel_put(x + 2, y + 4 + dy, color, texture);
			x++;
		}
		y++;
	}
}

void	render_slider_streaming(unsigned int *texture,
											float unit, int dy)
{
	int					x;
	int					y;

	y = 0;
	while (y < 4)
	{
		x = 0;
		while (x < 106)
		{
			button_pixel_put(x + 2, y + 6 + dy, 0x404040ff, texture);
			x++;
		}
		y++;
	}
	y = 0;
	while (y < 12)
	{
		x = 0;
		while (x < 6)
		{
			button_pixel_put(
				x + 2 + (100 * unit), y + 2 + dy, 0x666666ff, texture);
			x++;
		}
		y++;
	}
}

void	text(char *str)
{
	t_ui_state	*state;
	t_ivec2		moved;

	state = get_ui_state(NULL);
	moved = render_text(str, state->ui_text_x_offset, state->ui_text_y_pos);
	if (state->ui_max_width < state->ui_text_x_offset + moved.x)
		state->ui_max_width = state->ui_text_x_offset + moved.x;
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
}

int	button(int *var, char *str)
{
	t_window	*window;
	t_ui_state	*state;
	int			changed;

	window = get_window(NULL);
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 14;
	render_button_streaming(window->ui_texture_pixels, var, state->ui_text_y_pos);
	changed = edit_button_var(var, state);
	text(str);
	return (changed);
}

float	clamp(float var, float min, float max)
{
	if (var < min)
		var = min;
	else if (var > max)
		var = max;
	return (var);
}

void	int_slider(int *var, char *str, int min, int max)
{
	t_window	*window;
	t_ui_state	*state;
	float		unit;

	window = get_window(NULL);
	if (str)
		text(str);
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 14;
	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider_streaming(window->ui_texture_pixels, unit, state->ui_text_y_pos);
	edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
}

void	float_slider(float *var, char *str, float min, float max)
{
	t_window	*window;
	t_ui_state	*state;
	float		unit;

	window = get_window(NULL);
	if (str)
		text(str);
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 14;
	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider_streaming(window->ui_texture_pixels, unit, state->ui_text_y_pos);
	edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
}

void	file_save(char *str, char *extension, void (*f)(t_level *, char *))
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	if (call(str, NULL, NULL))
	{
		state->ui_location = UI_LOCATION_FILE_SAVE;
		ft_strcpy(state->extension, extension);
		state->open_file = *f;
	}
}

void	file_browser(char *str, char *extension, void (*f)(t_level *, char *))
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	if (call(str, NULL, NULL))
	{
		state->ui_location = UI_LOCATION_FILE_OPEN;
		ft_strcpy(state->extension, extension);
		state->open_file = *f;
	}
}

void	text_input(char *str, t_level *level)
{
	char	*filename;

	if (str[0])
		filename = ft_strjoin(str, ".doom-nukem");
	if ((!str[0] && call("input:", NULL, NULL))
		|| (str[0] && call(filename, NULL, NULL)))
		level->ui.state.text_input_enable = TRUE;
	if (str[0])
		free(filename);
}

int	call(char *str, void (*f)(t_level *), t_level *level)
{
	t_window	*window;
	int			res;
	t_ui_state	*state;
	int			color_tmp;
	t_ivec2		size;

	window = get_window(NULL);
	res = 0;
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 4;
	color_tmp = state->ui_text_color;
	state->ui_text_color = UI_BACKGROUND_COL;
	size = render_text(str, 4, state->ui_text_y_pos);
	if (state->ui_max_width < state->ui_text_x_offset + size.x)
		state->ui_max_width = state->ui_text_x_offset + size.x;
	state->ui_text_color = color_tmp;
	render_call_streaming(
		window->ui_texture_pixels, state->ui_text_y_pos, &size, state->ui_text_color);
	if (edit_call_var(state, size))
	{
		res = 1;
		if (*f)
			(*f)(level);
	}
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	return (res);
}