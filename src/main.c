/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/03 03:51:15by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

int		buttons(int button, const int pressed)
{
	static int		*keys;
	int				i;

	if (!(keys) && (i = -1))
	{
		if (!(keys = (int *)malloc(sizeof(int) * 4)))
			ft_error("memory allocation failed\n");
		while (++i < 4)
			keys[i] = -1;
	}
	i = -1;
	while (++i < 4)
		if (pressed == -1 && keys[i] == button)
			return (1);
		else if (pressed == 0 && keys[i] == button)
		{
			keys[i] = -1;
			return (0);
		}
		else if (pressed == 1 && keys[i] == -1)
		{
			keys[i] = button;
			return (0);
		}
	return (0);
}

void	action_loop(t_window *window, t_level *l)
{
	pthread_t	threads[THREAD_AMOUNT];
	t_rthread	**thread_data;
	int			window_horizontal_size;
	int			i;

	if (buttons(SDL_SCANCODE_A, -1))
		l->pos[0] -= 0.3;
	if (buttons(SDL_SCANCODE_D, -1))
		l->pos[0] += 0.3;
	if (buttons(SDL_SCANCODE_W, -1))
		l->pos[2] += 0.3;
	if (buttons(SDL_SCANCODE_S, -1))
		l->pos[2] -= 0.3;
	if (buttons(SDL_SCANCODE_Q, -1))
		l->angle -= 0.1;
	if (buttons(SDL_SCANCODE_E, -1))
		l->angle += 0.1;

	if (SDL_LockTexture(window->texture, NULL, (void**)&window->frame_buffer, &window_horizontal_size) != 0)
		ft_error("failed to lock texture\n");
	i = 0;
	if (!(thread_data = (t_rthread**)malloc(sizeof(t_rthread*) * THREAD_AMOUNT)))
		ft_error("memory allocation failed\n");
	while (i < THREAD_AMOUNT)
	{
		if (!(thread_data[i] = (t_rthread*)malloc(sizeof(t_rthread))))
			ft_error("memory allocation failed\n");
		thread_data[i]->id = i;
		thread_data[i]->level = l;
		thread_data[i]->window = window;
		pthread_create(&threads[i], NULL, rt_test, (void*)thread_data[i]);
		i++;
	}
	i = 0;
	while (i < THREAD_AMOUNT)
	{
		pthread_join(threads[i], NULL);
		free(thread_data[i]);
		i++;
	}
	free(thread_data);
	fill_pixels(window->frame_buffer, PIXEL_GAP);

	SDL_UnlockTexture(window->texture);
	SDL_RenderClear(window->SDLrenderer);
	SDL_RenderCopy(window->SDLrenderer, window->texture, NULL, NULL);
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

int			main(int argc, char **argv)
{
	SDL_Event	event;
	t_window	*window;
	t_level		*level;
	unsigned	frametime;

	level = rt_test_init_level();
	init_window(&window);
	while (1)
	{
		frametime = SDL_GetTicks();
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				return (0);
			else if (event.key.repeat == 0 && event.type == SDL_KEYDOWN)
				buttons(event.key.keysym.scancode, 1);
			else if (event.key.repeat == 0 && event.type == SDL_KEYUP)
				buttons(event.key.keysym.scancode, 0);
		}
		action_loop(window, level);
		frametime = SDL_GetTicks() - frametime;
		printf("time: %d ms\n", frametime);

		if (frametime < 100)
			usleep(10000);
	}
}
