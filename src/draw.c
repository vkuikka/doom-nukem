/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/04 14:03:28 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/03 04:25:51 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	draw_line(int line[4], t_window *window)
{
	float	x_increment;
	float	y_increment;
	float	xy[2];
	int		steps;
	int		i;

	i = 0;
	xy[0] = line[0];
	xy[1] = line[1];
	steps = abs(line[2] - line[0]) > abs(line[3] - line[1]) ?
			abs(line[2] - line[0]) : abs(line[3] - line[1]);
	if (!steps)
		return ;
	x_increment = (line[2] - line[0]) / (float)steps;
	y_increment = (line[3] - line[1]) / (float)steps;
	while (i <= (steps))
	{
		SDL_RenderDrawPoint(window->SDLrenderer, xy[0], xy[1]);
		xy[0] += x_increment;
		xy[1] += y_increment;
		i++;
	}
}

