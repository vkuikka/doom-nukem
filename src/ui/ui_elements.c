/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_elements.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/13 12:51:47 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/12 21:07:26 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

t_level	*get_level(t_level *get_level)
{
	static t_level	*level = NULL;

	if (get_level)
		level = get_level;
	else
		return (level);
	return (NULL);
}

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
		*unit = (float)(x - 2)
			/ (float)(UI_SLIDER_WIDTH - UI_SLIDER_BUTTON_WIDTH);
		return (TRUE);
	}
	return (FALSE);
}

static int	edit_call_var(t_ui_state *state, t_rect rect)
{
	if (state->mouse_location == MOUSE_LOCATION_UI && state->m1_click
		&& mouse_collision(rect, state->mouse))
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

static t_rect	render_call(t_ivec2 *size, t_ui_state *state)
{
	t_window	*window;
	t_rect		rect;
	int			color;
	int			x;
	int			y;

	window = get_window(NULL);
	rect.x = state->ui_text_x_offset - 2;
	rect.y = state->ui_text_y_pos + 1;
	rect.w = size->x + 4;
	rect.h = size->y - 1;
	color = state->ui_text_color;
	if (mouse_collision(rect, state->mouse))
		color = set_lightness(color, -0.1);
	y = rect.y - 1;
	while (++y < rect.y + rect.h)
	{
		x = rect.x - 1;
		while (++x < rect.x + rect.w)
			button_pixel_put(x, y, color, window->ui_texture_pixels);
	}
	return (rect);
}

static void	render_button(unsigned int *texture, int *var, int dy, int color)
{
	int	edge_color;
	int	x;
	int	y;

	edge_color = set_lightness(color, -0.8);
	if (!(*var))
		color = set_lightness(color, -0.7);
	y = 0;
	while (y < 10)
	{
		x = 0;
		while (x < 10)
		{
			if (y < 1 || y > 8 || x < 1 || x > 8)
				button_pixel_put(x + 2, y + 4 + dy, edge_color, texture);
			else
				button_pixel_put(x + 2, y + 4 + dy, color, texture);
			x++;
		}
		y++;
	}
}

static void	render_slider_button(unsigned int *texture, float pos,
								int dy, int color)
{
	int	x;
	int	y;

	y = 0;
	while (y < UI_SLIDER_BUTTON_HEIGHT)
	{
		x = 0;
		while (x <= UI_SLIDER_BUTTON_WIDTH)
		{
			button_pixel_put(x + 2
				+ (int)((UI_SLIDER_WIDTH - UI_SLIDER_BUTTON_WIDTH)
					* pos), y + 2 + dy, color, texture);
			x++;
		}
		y++;
	}
}

static void	render_color_slider(t_window *window, float pos,
						int dy, unsigned int *colors)
{
	int	x;
	int	y;

	y = -1;
	while (++y < 4)
	{
		x = -1;
		while (++x < UI_SLIDER_WIDTH)
			button_pixel_put(x + 2, y + 6 + dy, colors[x],
				window->ui_texture_pixels);
	}
	render_slider_button(window->ui_texture_pixels, pos, dy,
							colors[(int)(pos * (UI_SLIDER_WIDTH - 1))]);
}

static void	render_slider(unsigned int *texture, float pos, int dy, int color)
{
	int	x;
	int	y;
	int	color2;

	color2 = set_lightness(color, -0.5);
	y = -1;
	while (++y < 4)
	{
		x = -1;
		while (++x < UI_SLIDER_WIDTH)
			button_pixel_put(x + 2, y + 6 + dy, color2, texture);
	}
	render_slider_button(texture, pos, dy, color);
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
	render_button(window->ui_texture_pixels, var,
		state->ui_text_y_pos, state->ui_text_color);
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

int	int_slider(int *var, char *str, int min, int max)
{
	t_window	*window;
	t_ui_state	*state;
	float		unit;
	int			res;

	window = get_window(NULL);
	if (str)
		text(str);
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 14;
	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider(window->ui_texture_pixels, unit,
		state->ui_text_y_pos, state->ui_text_color);
	res = edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	return (res);
}

int	float_slider(float *var, char *str, float min, float max)
{
	t_window	*window;
	t_ui_state	*state;
	float		unit;
	int			res;

	window = get_window(NULL);
	if (str)
		text(str);
	state = get_ui_state(NULL);
	state->ui_text_x_offset = 14;
	*var = clamp(*var, min, max);
	*var -= min;
	unit = (float)*var / (float)(max - min);
	render_slider(window->ui_texture_pixels, unit,
		state->ui_text_y_pos, state->ui_text_color);
	res = edit_slider_var(&unit, state);
	unit = clamp(unit, 0, 1);
	*var = min + ((max - min) * unit);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	return (res);
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

int	color_slider(t_color_hsl *var, char *str)
{
	t_ui_state		*state;
	unsigned int	colors[UI_SLIDER_WIDTH];
	int				res;

	state = get_ui_state(NULL);
	text(str);
	render_color_slider(get_window(NULL), var->hue,
		state->ui_text_y_pos, state->color_slider_hue_colors);
	res = edit_slider_var(&var->hue, state);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	generate_color_slider_saturation(&colors[0], var->rgb_hue);
	render_color_slider(get_window(NULL), var->saturation,
		state->ui_text_y_pos, &colors[0]);
	res += edit_slider_var(&var->saturation, state);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	var->lightness = (var->lightness + 1) / 2;
	generate_color_slider_lightness(&colors[0], var->rgb_hue);
	render_color_slider(get_window(NULL), var->lightness,
		state->ui_text_y_pos, &colors[0]);
	res += edit_slider_var(&var->lightness, state);
	var->lightness = var->lightness * 2 - 1;
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	hsl_update_color(var);
	return (res);
}

void	file_save(char *str, char *extension, void (*f)(t_level *, char *))
{
	t_ui_state	*state;

	state = get_ui_state(NULL);
	if (call(str, NULL))
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
	if (call(str, NULL))
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
		if (call("input:", NULL))
			level->ui.state.text_input_enable = TRUE;
		return ;
	}
	filename = ft_strjoin(str, ".doom-nukem");
	if (call(filename, NULL))
		level->ui.state.text_input_enable = TRUE;
	free(filename);
}

int	call(char *str, void (*f)(t_level *))
{
	t_ui_state	*state;
	t_level		*level;
	t_ivec2		size;
	t_rect		rect;
	int			tmp;

	state = get_ui_state(NULL);
	level = get_level(NULL);
	state->ui_text_x_offset = 4;
	tmp = state->ui_text_color;
	state->ui_text_color = UI_BACKGROUND_COL;
	size = render_text(str, 4, state->ui_text_y_pos);
	state->ui_text_color = tmp;
	if (state->ui_max_width < state->ui_text_x_offset + size.x)
		state->ui_max_width = state->ui_text_x_offset + size.x;
	rect = render_call(&size, state);
	tmp = edit_call_var(state, rect);
	if (tmp && *f)
		(*f)(level);
	state->ui_text_y_pos += UI_ELEMENT_HEIGHT;
	return (tmp);
}
