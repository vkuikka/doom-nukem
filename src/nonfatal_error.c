/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nonfatal_error.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/11 16:04:11 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/11 16:04:49 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void		nonfatal_error(t_level *level, char *message)
{
	int i;

	level->ui.state.error_amount++;
	i = level->ui.state.error_amount;
	if (!(level->ui.state.error_start_time = (unsigned*)realloc(level->ui.state.error_start_time, sizeof(unsigned) * (i))))
		ft_error("memory allocation failed\n");
	level->ui.state.error_start_time[i - 1] = SDL_GetTicks();
	if (!(level->ui.state.error_message = (char**)realloc(level->ui.state.error_message, sizeof(char*) * (i))))
		ft_error("memory allocation failed\n");
	if (!(level->ui.state.error_message[i - 1] = (char*)malloc(sizeof(char) * ft_strlen(message) + 1)))
		ft_error("memory allocation failed\n");
	ft_strcpy(level->ui.state.error_message[i - 1], message);
}

static void	ui_remove_expired_nonfatal_errors(t_level *level)
{
	int i;

	i = 0;
	while (i < level->ui.state.error_amount)
	{
		if (level->ui.state.error_start_time[i] + 1000 * NONFATAL_ERROR_LIFETIME_SECONDS < SDL_GetTicks())
		{
			free(level->ui.state.error_message[i]);
			for (int k = i; k < level->ui.state.error_amount - 1; k++)
			{
				level->ui.state.error_message[k] = level->ui.state.error_message[k + 1];
				level->ui.state.error_start_time[k] = level->ui.state.error_start_time[k + 1];
			}
			level->ui.state.error_amount--;
			i = -1;
		}
		i++;
	}
	i = level->ui.state.error_amount;
	if (i)
	{
		level->ui.state.error_start_time = (unsigned*)realloc(level->ui.state.error_start_time, sizeof(unsigned) * i);
		level->ui.state.error_message = (char**)realloc(level->ui.state.error_message, sizeof(char*) * i);
	}
	else
	{
		free(level->ui.state.error_start_time);
		free(level->ui.state.error_message);
		level->ui.state.error_start_time = NULL;
		level->ui.state.error_message = NULL;
	}
}

static int ui_nonfatal_get_fade(t_level *level, int i)
{
	unsigned time;

	time = level->ui.state.error_start_time[i] + 1000 * NONFATAL_ERROR_LIFETIME_SECONDS - SDL_GetTicks();
	if (time < NONFATAL_ERROR_FADEOUT_TIME_MS)
		return (0xff * (time / (float)NONFATAL_ERROR_FADEOUT_TIME_MS));
	return (0xff);
}

void	ui_render_nonfatal_errors(SDL_Texture *texture, t_window *window, t_level *level)
{
	t_ivec2	rect;
	int		i;

	if (!level->ui.state.error_amount)
		return ;
	ui_remove_expired_nonfatal_errors(level);
	if (!level->ui.state.error_amount)
		return ;
	rect.x = level->ui.state.ui_max_width + UI_PADDING_4 + UI_PADDING_4;
	rect.y = RES_Y / 2;
	i = 0;
	while (i < level->ui.state.error_amount)
	{
		set_text_color(UI_ERROR_COLOR + ui_nonfatal_get_fade(level, i));
		put_text(level->ui.state.error_message[i], window, texture, rect);
		rect.y += UI_ELEMENT_HEIGHT;
		i++;
	}
}
