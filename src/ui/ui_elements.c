/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_elements.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/13 12:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/31 12:39:19 by rpehkone         ###   ########.fr       */
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

static int	edit_slider_var(float *unit, t_ui_state *state)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_drag
		&& y >= state->ui_text_y_pos + 4 && y <= state->ui_text_y_pos + 15
		&& x < UI_SLIDER_WIDTH - 2 && x >= 2)
	{
		*unit = (float)(x - 2) / (float)(UI_SLIDER_WIDTH - UI_SLIDER_BUTTON_WIDTH);
		return (TRUE);
	}
	return (FALSE);
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

void	render_color_slider(unsigned int *texture, float pos,
						int dy, unsigned int *colors)
{
	int	x;
	int	y;

	y = -1;
	while (++y < 4)
	{
		x = 0;
		while (x < UI_SLIDER_WIDTH)
		{
			button_pixel_put(x + 2, y + 6 + dy, colors[x], texture);
			x++;
		}
	}
	y = 0;
	while (y < 12)
	{
		x = 0;
		while (x <= UI_SLIDER_BUTTON_WIDTH)
		{
			button_pixel_put(
				x + 2 + ((UI_SLIDER_WIDTH - UI_SLIDER_BUTTON_WIDTH) * pos), y + 2 + dy, 0x666666ff, texture);
			x++;
		}
		y++;
	}
}

void	render_slider_streaming(unsigned int *texture,
											float unit, int dy)
{
	int	x;
	int	y;

	y = -1;
	while (++y < 4)
	{
		x = 0;
		while (x < UI_SLIDER_WIDTH)
		{
			button_pixel_put(x + 2, y + 6 + dy, 0x404040ff, texture);
			x++;
		}
	}
	y = 0;
	while (y < 12)
	{
		x = 0;
		while (x <= UI_SLIDER_BUTTON_WIDTH)
		{
			button_pixel_put(
				x + 2 + ((UI_SLIDER_WIDTH - UI_SLIDER_BUTTON_WIDTH) * unit), y + 2 + dy, 0x666666ff, texture);
			x++;
		}
		y++;
	}
}

void	text(char *str)
{
	t_ui_state	*state;
	t_ivec2		moved;

	if (!str)
		return ;
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
	render_button_streaming(window->ui_texture_pixels, var,
		state->ui_text_y_pos);
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
	render_slider_streaming(window->ui_texture_pixels, unit,
		state->ui_text_y_pos);
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
	render_slider_streaming(window->ui_texture_pixels, unit,
		state->ui_text_y_pos);
	edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
}

void	generate_color_slider_saturation(unsigned int *res, int color)
{
	int	i;

	i = 0;
	while (i < UI_SLIDER_WIDTH)
	{
		res[i] = set_saturation(color, i / (float)UI_SLIDER_WIDTH);
		i++;
	}
}

void	generate_color_slider_lightness(unsigned int *res, int color)
{
	int	i;

	i = 0;
	while (i < UI_SLIDER_WIDTH)
	{
		res[i] = set_lightness(color, (i / (float)UI_SLIDER_WIDTH) * 2 - 1);
		i++;
	}
}

void	color_slider(t_color_hsl *var, char *str)
{
	t_window		*window;
	t_ui_state		*state;
	unsigned int	colors[UI_SLIDER_WIDTH];

	window = get_window(NULL);
	state = get_ui_state(NULL);
	text(str);
	render_color_slider(window->ui_texture_pixels, var->hue,
		state->ui_text_y_pos, state->color_slider_hue_colors);
	edit_slider_var(&var->hue, state);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	generate_color_slider_saturation(&colors[0], var->rgb_hue);
	render_color_slider(window->ui_texture_pixels, var->saturation,
		state->ui_text_y_pos, &colors[0]);
	edit_slider_var(&var->saturation, state);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	var->lightness = (var->lightness + 1) / 2;
	generate_color_slider_lightness(&colors[0], var->rgb_hue);
	render_color_slider(window->ui_texture_pixels, var->lightness,
		state->ui_text_y_pos, &colors[0]);
	edit_slider_var(&var->lightness, state);
	var->lightness = var->lightness * 2 - 1;
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	hsl_update_color(var);
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

	if (!str[0])
	{
		if (call("input:", NULL, NULL))
			level->ui.state.text_input_enable = TRUE;
		return ;
	}
	filename = ft_strjoin(str, ".doom-nukem");
	if (call(filename, NULL, NULL))
		level->ui.state.text_input_enable = TRUE;
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
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 4;
	color_tmp = state->ui_text_color;
	state->ui_text_color = UI_BACKGROUND_COL;
	size = render_text(str, 4, state->ui_text_y_pos);
	if (state->ui_max_width < state->ui_text_x_offset + size.x)
		state->ui_max_width = state->ui_text_x_offset + size.x;
	state->ui_text_color = color_tmp;
	render_call_streaming(window->ui_texture_pixels, state->ui_text_y_pos,
		&size, state->ui_text_color);
	res = edit_call_var(state, size);
	if (res && *f)
		(*f)(level);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	return (res);
}
