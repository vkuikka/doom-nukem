/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_raycast.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 11:42:17 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/20 23:01:38 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void	wait_threads(t_level *level, SDL_Thread *threads[THREAD_AMOUNT])
{
	int	thread_casts;
	int	i;

	i = 0;
	while (i < THREAD_AMOUNT)
	{
		SDL_WaitThread(threads[i], &thread_casts);
		level->ui.total_raycasts += thread_casts;
		i++;
	}
}

static void	create_threads(t_level *level, t_window *window)
{
	SDL_Thread	*threads[THREAD_AMOUNT];
	t_rthread	thread_data[THREAD_AMOUNT];
	int			i;

	i = 0;
	while (i < THREAD_AMOUNT)
	{
		thread_data[i].id = i;
		thread_data[i].level = level;
		thread_data[i].window = window;
		threads[i] = SDL_CreateThread(init_raycast, "raycast",
				(void *)&thread_data[i]);
		i++;
	}
	wait_threads(level, threads);
}

void	render_raycast(t_window *window, t_level *level,
								t_game_state *game_state)
{
	if (SDL_LockTexture(window->texture, NULL,
			(void **)&window->frame_buffer, &(int){0}) != 0)
		ft_error("failed to lock texture\n");
	if (level->render_is_first_pass || *game_state != GAME_STATE_INGAME)
		memset(window->frame_buffer, 0, RES_X * RES_Y * sizeof(int));
	if (level->ui.bloom_intensity)
		memset(window->brightness_buffer, 0, RES_X * RES_Y * sizeof(t_color));
	create_threads(level, window);
}
