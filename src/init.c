/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 14:38:45 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/13 00:26:12 by vkuikka          ###   ########.fr       */
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

void			init_level(t_level **res)
{
	t_level		*level;

	if (!(level = (t_level *)malloc(sizeof(t_level))))
		ft_error("memory allocation failed\n");
	ft_bzero(level, sizeof(t_level));
	level->cam.pos.x = 0;
	level->cam.pos.y = -5;
	level->cam.pos.z = 0;
	level->cam.look_side = 0;
	level->cam.look_up = 0;
	level->shadow_color = 0;

	// load_obj("level/two.obj", &level->all);
	// load_obj("level/test.obj", &level->all);
	// load_obj("level/cube.obj", &level->all);
	// load_obj("level/island.obj", &level->all);
	// load_obj("level/cache.obj", &level->all);
	load_obj("level/ship.obj", &level->all);
	// load_obj("level/one_tri.obj", &level->all);
	// load_obj("level/tri_test.obj", &level->all);
	// load_obj("level/torus.obj", &level->all);
	// load_obj("level/monkey.obj", &level->all);
	// load_obj("level/teapot_decimated.obj", &level->all);

	level->texture = bmp_read("out.bmp");
	level->normal_map = bmp_read("normal.bmp");
	load_obj("skybox.obj", &level->sky.all);
	load_obj("skybox.obj", &level->sky.visible);
	level->sky.img = bmp_read("skybox.bmp");

	if (!(level->visible.tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	*res = level;
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

void			init_enemy(t_tri *face)
{
	face->isenemy = 1;
	if (!(face->enemy = (t_enemy*)malloc(sizeof(t_enemy))))
		ft_error("memory allocation failed");
	face->enemy->move_speed = 2;
	face->enemy->dist_limit = 1;
	face->enemy->initial_health = 100;
	face->enemy->remaining_health = face->enemy->initial_health;
	face->enemy->projectile_speed = 0;
	face->enemy->attack_range = 1.5;
	face->enemy->attack_frequency = 0.5;
	face->enemy->attack_damage = 10;
	face->enemy->current_attack_delay = 0;
}
