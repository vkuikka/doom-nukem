/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 14:38:45 by vkuikka           #+#    #+#             */
/*   Updated: 2021/04/01 15:16:35 by vkuikka          ###   ########.fr       */
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

t_level			*init_level(void)
{
	t_level		*level;

	global_seginfo = "init_level\n";
	if (!(level = (t_level *)malloc(sizeof(t_level))))
		ft_error("memory allocation failed\n");

	level->pos.x = 0;
	level->pos.y = -5;
	level->pos.z = 0;
	level->look_side = 0;
	level->look_up = 0;
	level->sun_contrast = 0;	//max 1
	level->shadow_color = 0;
	level->direct_shadow_contrast = 0;	//max 1

	level->sun_dir.x = 1;
	level->sun_dir.y = 1;
	level->sun_dir.z = 1;
	vec_normalize(&level->sun_dir);

	global_seginfo = "load_obj\n";
	// load_obj("level/two.obj", &level->all);
	// load_obj("level/test.obj", &level->all);
	// load_obj("level/cube.obj", &level->all);
	// load_obj("level/island.obj", &level->all);
	// load_obj("level/cache.obj", &level->all);
	// load_obj("level/ship.obj", &level->all);
	load_obj("level/ship_2.obj", &level->all);
	// load_obj("level/one_tri.obj", &level->all);
	// load_obj("level/tri_test.obj", &level->all);
	// load_obj("level/torus.obj", &level->all);
	// load_obj("level/monkey.obj", &level->all);
	// load_obj("level/teapot_decimated.obj", &level->all);

	global_seginfo = "load skybox obj\n";
	load_obj("skybox.obj", &level->sky.obj);
	global_seginfo = "load skybox texture\n";
	level->sky.img = bmp_read("skybox.bmp");

	if (!(level->visible.tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	if (!(level->ssp = (t_obj*)malloc(sizeof(t_obj) * 2)))
		ft_error("memory allocation failed\n");
	if (!(level->ssp[0].tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	if (!(level->ssp[1].tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	return (level);
}

void			init_window(t_window **window)
{
	global_seginfo = "init_window\n";
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
	// if (!(window[0]->index_buffer = (int *)malloc(sizeof(int) * (RES_X * RES_Y))))
	// 	ft_error("memory allocation failed\n");
}
