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

void	action_loop(t_window *window, t_world world, SDL_Texture *txt)
{
	static float posz = 0;
	static float posx = 0;
	static float angle = 0;

	if (buttons(SDL_SCANCODE_W, -1))
		posz += 0.3;
	if (buttons(SDL_SCANCODE_S, -1))
		posz -= 0.3;
	if (buttons(SDL_SCANCODE_A, -1))
		posx -= 0.3;
	if (buttons(SDL_SCANCODE_D, -1))
		posx += 0.3;
	if (buttons(SDL_SCANCODE_Q, -1))
		angle -= 0.1;
	if (buttons(SDL_SCANCODE_E, -1))
		angle += 0.1;


	SDL_SetRenderTarget(window->SDLrenderer, txt);
	SDL_SetRenderDrawColor(window->SDLrenderer, 0, 0, 0, 255);
	SDL_RenderClear(window->SDLrenderer);
	SDL_SetRenderDrawColor(window->SDLrenderer, 250, 250, 250, 255);

	rt_test(window, posz, posx, angle);

	SDL_SetRenderTarget(window->SDLrenderer, NULL);
	SDL_RenderCopy(window->SDLrenderer, txt, NULL, NULL);
	SDL_RenderPresent(window->SDLrenderer);
	return ;
}

int			main(int argc, char **argv)
{
	SDL_Texture *txt;
	SDL_Event	event;
	t_window	*window;
	t_world		world;
	unsigned	frametime;

	init_window(&window, &txt);
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
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_SetRenderTarget(window->SDLrenderer, txt);
				SDL_SetRenderDrawColor(window->SDLrenderer, 0, 0, 0, 255);
				SDL_RenderClear(window->SDLrenderer);
				SDL_SetRenderDrawColor(window->SDLrenderer, 250, 250, 250, 255);

				draw_line((int[4]){0, 0, event.motion.x, event.motion.y}, window);	//test

				SDL_SetRenderTarget(window->SDLrenderer, NULL);
				SDL_RenderCopy(window->SDLrenderer, txt, NULL, NULL);
				SDL_RenderPresent(window->SDLrenderer);
			}
		}
		action_loop(window, world, txt);
		frametime = SDL_GetTicks() - frametime;
		// printf("frametime: %d ms\n", frametime);

		if (frametime < 1000)
			usleep(1000);
	}
}
