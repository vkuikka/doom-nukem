/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 14:38:45 by vkuikka           #+#    #+#             */
/*   Updated: 2021/01/26 02:26:58 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

SDL_Texture		*empty_texture(SDL_Renderer *renderer)
{
	SDL_Texture *texture = NULL;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);
	return (texture);
}

t_level			*init_level()
{
	t_level		*l;

	if (!(l = (t_level *)malloc(sizeof(t_level))) ||
		!(l->obj = (t_obj *)malloc(sizeof(t_obj) * 1)))
		ft_error("memory allocation failed\n");

	l->pos[0] = 0;
	l->pos[1] = 0;
	l->pos[2] = 0;
	l->look_side = 0;
	l->look_up = 0.5;
	l->txtr = NULL;

	// load_obj("level/two.obj", &l->obj[0]);
	// load_obj("level/test.obj", &l->obj[0]);
	// load_obj("level/cube.obj", &l->obj[0]);
	// load_obj("level/island.obj", &l->obj[0]);
	// load_obj("level/cache.obj", &l->obj[0]);
	load_obj("level/ship.obj", &l->obj[0]);
	// load_obj("level/one_tri.obj", &l->obj[0]);
	// load_obj("level/tri_test.obj", &l->obj[0]);
	// load_obj("level/torus.obj", &l->obj[0]);
	// load_obj("level/monkey.obj", &l->obj[0]);
	// load_obj("level/teapot_decimated.obj", &l->obj[0]);

	return (l);
}

void			init_window(t_window **window)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))// || !IMG_Init(IMG_INIT_PNG))
		ft_error("could not initialize SDL\n");
	if (!(*window = (t_window *)malloc(sizeof(t_window))))
		ft_error("memory allocation failed\n");
	if (!(window[0]->SDLwindow = SDL_CreateWindow("DOOM NUKEM", SDL_WINDOWPOS_CENTERED,
							SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0)))
		ft_error("could not create window");
	if (!(window[0]->SDLrenderer = SDL_CreateRenderer(window[0]->SDLwindow, -1, 0)))
		ft_error("could not create renderer");
	window[0]->texture = empty_texture(window[0]->SDLrenderer);
	window[0]->frame_buffer = NULL;
	if (!(window[0]->depth_buffer = (float *)malloc(sizeof(float) * (RES_X * RES_Y))))
		ft_error("memory allocation failed\n");
}
