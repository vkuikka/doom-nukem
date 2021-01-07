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

void	action_loop(t_window *window, t_world world)
{
	static t_player	p;
	int				i;

	if (buttons(SDL_SCANCODE_W, -1))
		p.posz += 0.3;
	if (buttons(SDL_SCANCODE_S, -1))
		p.posz -= 0.3;
	if (buttons(SDL_SCANCODE_A, -1))
		p.posx -= 0.3;
	if (buttons(SDL_SCANCODE_D, -1))
		p.posx += 0.3;
	if (buttons(SDL_SCANCODE_Q, -1))
		p.angle -= 0.1;
	if (buttons(SDL_SCANCODE_E, -1))
		p.angle += 0.1;


	int pitch;
	if(SDL_LockTexture(window->texture, NULL, (void**)&window->pixels, &pitch ) != 0)
		ft_error("failed to lock texture\n");
	i = 0;
	pthread_t	tid[THREAD_AMOUNT];
	t_rthread	**th;
	if (!(th = (t_rthread**)malloc(sizeof(t_rthread*) * THREAD_AMOUNT)))
		ft_error("memory allocation failed\n");
	while (i < THREAD_AMOUNT)
	{
		if (!(th[i] = (t_rthread*)malloc(sizeof(t_rthread))))
			ft_error("memory allocation failed\n");
		if (!(th[i]->player = (t_player*)malloc(sizeof(t_player))))
			ft_error("memory allocation failed\n");
		th[i]->id = i;
		th[i]->player->posz = p.posz;
		th[i]->player->posx = p.posx;
		th[i]->player->angle = p.angle;
		th[i]->window = window;
		pthread_create(&tid[i], NULL, rt_test, (void*)th[i]);
		i++;
	}
	rt_test(th[0]);
	i = 0;
	while (i < THREAD_AMOUNT)
	{
		if (i != 0)
			pthread_join(tid[i], NULL);
		i++;
	}
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
	t_world		world;
	unsigned	frametime;

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
		action_loop(window, world);
		frametime = SDL_GetTicks() - frametime;
		printf("time: %d ms\n", frametime);

		if (frametime < 100)
			usleep(10000);
	}
}
