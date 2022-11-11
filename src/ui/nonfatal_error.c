/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nonfatal_error_0.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	nonfatal_error(char *message)
{
	t_level	*level;
	int		i;

	level = get_level(NULL);
	level->ui.state.error_amount++;
	i = level->ui.state.error_amount;
	level->ui.state.error_start_time = (unsigned int *)realloc(
			level->ui.state.error_start_time, sizeof(unsigned int) * (i));
	if (!level->ui.state.error_start_time)
		ft_error("memory allocation failed\n");
	level->ui.state.error_start_time[i - 1] = SDL_GetTicks();
	level->ui.state.error_message = (char **)realloc(
			level->ui.state.error_message, sizeof(char *) * (i));
	if (!level->ui.state.error_message)
		ft_error("memory allocation failed\n");
	level->ui.state.error_message[i - 1]
		= (char *)malloc(sizeof(char) * ft_strlen(message) + 1);
	if (!level->ui.state.error_message[i - 1])
		ft_error("memory allocation failed\n");
	ft_strcpy(level->ui.state.error_message[i - 1], message);
}

void	ui_remove_expired_nonfatal_start_times(t_level *level)
{
	int	i;

	i = level->ui.state.error_amount;
	if (i)
	{
		level->ui.state.error_start_time = (unsigned int *)realloc(
				level->ui.state.error_start_time, sizeof(unsigned int) * i);
		level->ui.state.error_message = (char **)realloc(
				level->ui.state.error_message, sizeof(char *) * i);
	}
	else
	{
		free(level->ui.state.error_start_time);
		free(level->ui.state.error_message);
		level->ui.state.error_start_time = NULL;
		level->ui.state.error_message = NULL;
	}
}

void	ui_remove_expired_nonfatal_errors(t_level *level)
{
	int	i;
	int	k;

	i = -1;
	while (++i < level->ui.state.error_amount)
	{
		if (level->ui.state.error_start_time[i]
			+ 1000 * NONFATAL_ERROR_LIFETIME_SECONDS
			< SDL_GetTicks())
		{
			free(level->ui.state.error_message[i]);
			k = i;
			while (k < level->ui.state.error_amount - 1)
			{
				level->ui.state.error_message[k]
					= level->ui.state.error_message[k + 1];
				level->ui.state.error_start_time[k]
					= level->ui.state.error_start_time[k + 1];
				k++;
			}
			level->ui.state.error_amount--;
			i = -1;
		}
	}
	ui_remove_expired_nonfatal_start_times(level);
}

int	ui_nonfatal_get_fade(t_level *level, int i)
{
	unsigned int	time;

	time = level->ui.state.error_start_time[i]
		+ 1000 * NONFATAL_ERROR_LIFETIME_SECONDS - SDL_GetTicks();
	if (time < NONFATAL_ERROR_FADEOUT_TIME_MS)
		return (0xff * (time / (float)NONFATAL_ERROR_FADEOUT_TIME_MS));
	return (0xff);
}

void	ui_render_nonfatal_errors(t_level *level)
{
	int		x;
	int		y;
	int		i;

	if (!level->ui.state.error_amount)
		return ;
	ui_remove_expired_nonfatal_errors(level);
	if (!level->ui.state.error_amount)
		return ;
	x = level->ui.state.ui_max_width + UI_PADDING_4 + UI_PADDING_4;
	y = RES_Y / 2;
	i = 0;
	level->ui.state.current_font = level->ui.editor_font;
	while (i < level->ui.state.error_amount)
	{
		set_text_color(UI_ERROR_COLOR + ui_nonfatal_get_fade(level, i));
		render_text(level->ui.state.error_message[i], x, y);
		y += UI_ELEMENT_HEIGHT;
		i++;
	}
}
