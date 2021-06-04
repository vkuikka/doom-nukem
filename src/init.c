/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 14:38:45 by vkuikka           #+#    #+#             */
/*   Updated: 2021/05/21 17:15:10y vkuikka          ###   ########.fr       */
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
	level->player_health = PLAYER_HEALTH_MAX;
	level->player_ammo = PLAYER_AMMO_MAX;
	level->cam.pos.x = 0;
	level->cam.pos.y = -5;
	level->cam.pos.z = 0;
	level->cam.look_side = 0;
	level->cam.look_up = 0;
	level->main_menu_anim_time = 2;
	level->world_brightness = 0.15;

	load_obj("level/ship.obj", &level->all);

	level->texture = bmp_read("out.bmp");
	if (!(level->baked = (t_color *)malloc(sizeof(t_color) * (level->texture.width * level->texture.height))))
		ft_error("memory allocation failed\n");
	if (!(level->spray_overlay = (unsigned *)malloc(sizeof(unsigned) * (level->texture.width * level->texture.height))))
		ft_error("memory allocation failed\n");
	ft_bzero(level->spray_overlay, level->texture.width * level->texture.height * 4);
	level->bake_status = BAKE_NOT_BAKED;

	level->normal_map = bmp_read("normal.bmp");
	load_obj("embed/skybox.obj", &level->sky.all);
	load_obj("embed/skybox.obj", &level->sky.visible);
	level->sky.img = bmp_read("skybox.bmp");
	level->spray = bmp_read("spray.bmp");

	level->main_menu_title = bmp_read("embed/title.bmp");

	char viewmodel_name[] = "embed/viewmodel/m4_0.bmp";
	for (int i = 0; i < VIEWMODEL_FRAMES; i++)
	{
		viewmodel_name[19] = '0' + i;
		level->viewmodel[i] = bmp_read(viewmodel_name);
	}

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
	ft_bzero(face->enemy, sizeof(t_enemy));
	face->enemy->move_speed = 2;
	face->enemy->dist_limit = 1;
	face->enemy->initial_health = 100;
	face->enemy->remaining_health = face->enemy->initial_health;
	face->enemy->projectile_speed = 0;
	face->enemy->projectile_scale = 1;
	face->enemy->attack_range = 1.5;
	face->enemy->attack_frequency = 0.5;
	face->enemy->attack_damage = 10;
	face->enemy->current_attack_delay = 0;
	face->enemy->projectile_uv[0].x = .5;
	face->enemy->projectile_uv[0].y = 0;
	face->enemy->projectile_uv[1].x = 0;
	face->enemy->projectile_uv[1].y = 1;
	face->enemy->projectile_uv[2].x = 1;
	face->enemy->projectile_uv[2].y = 1;
}

void			init_player(t_enemy *player)
{
	player->move_speed = RUN_SPEED;
	player->initial_health = 100;
	player->remaining_health = player->initial_health;
	player->projectile_speed = 40;
	player->projectile_scale = 0.5;
	player->attack_frequency = 0.5;
	player->attack_damage = -10;
	player->current_attack_delay = 0;
	player->projectile_uv[0].x = .5;
	player->projectile_uv[0].y = 0;
	player->projectile_uv[1].x = 0;
	player->projectile_uv[1].y = 1;
	player->projectile_uv[2].x = 1;
	player->projectile_uv[2].y = 1;
}

void	init_audio(t_level *l)
{
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096) < 0)
		ft_error("Failed to initialize SDL_Mixer");
	l->audio.music_volume = AUDIO_VOLUME_INIT;
	l->audio.sound_effect_volume = AUDIO_VOLUME_INIT;
	Mix_VolumeMusic(l->audio.music_volume);
	Mix_Volume(-1, l->audio.sound_effect_volume);
	if (!(l->audio.music = Mix_LoadMUS(AUDIO_MUSIC)))
		ft_error(AUDIO_MUSIC);
	if (!(l->audio.jump = Mix_LoadWAV(AUDIO_JUMP)))
		ft_error(AUDIO_JUMP);
	if (!(l->audio.gunshot = Mix_LoadWAV(AUDIO_GUNSHOT)))
		ft_error(AUDIO_GUNSHOT);
	if (!(l->audio.reload = Mix_LoadWAV(AUDIO_RELOAD)))
		ft_error(AUDIO_RELOAD);
	if (!(l->audio.death = Mix_LoadWAV(AUDIO_DEATH)))
		ft_error(AUDIO_DEATH);
	if (!(l->audio.door = Mix_LoadWAV(AUDIO_DOOR)))
		ft_error(AUDIO_DOOR);
}
