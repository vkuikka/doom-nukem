/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 14:38:45 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/13 18:03:38 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

SDL_Texture	*empty_texture(SDL_Renderer *renderer)
{
	SDL_Texture	*texture;

	texture = NULL;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
	SDL_SetRenderTarget(renderer, texture);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);
	return (texture);
}

t_level	*init_level(void)
{
	t_level	*level;
	int		i;
	char	viewmodel_name[] = "embed/viewmodel/ak_0.bmp";

	level = (t_level *)malloc(sizeof(t_level));
	if (!level)
		ft_error("memory allocation failed\n");
	ft_bzero(level, sizeof(t_level));
	level->player_health = PLAYER_HEALTH_MAX;
	level->player_ammo = PLAYER_AMMO_MAX;
	level->win_dist = INITIAL_LEVEL_WIN_DIST;
	level->cam.pos.x = 0;
	level->cam.pos.y = -PLAYER_HEIGHT;
	level->cam.pos.z = 0;
	level->cam.look_side = 0;
	level->cam.look_up = 0;
	level->main_menu_anim_time = 2;
	level->world_brightness = 0.15;
	level->texture = bmp_read("out.bmp");
	level->baked = (t_color *)malloc(sizeof(t_color)
			* (level->texture.width * level->texture.height));
	if (!level->baked)
		ft_error("memory allocation failed\n");
	level->spray_overlay = (unsigned int *)malloc(sizeof(unsigned int)
			* (level->texture.width * level->texture.height));
	if (!level->spray_overlay)
		ft_error("memory allocation failed\n");
	ft_bzero(level->spray_overlay,
		level->texture.width * level->texture.height * 4);
	level->bake_status = BAKE_NOT_BAKED;
	level->normal_map = bmp_read("normal.bmp");
	load_obj("embed/skybox.obj", &level->sky.all);
	load_obj("embed/skybox.obj", &level->sky.visible);
	level->sky.img = bmp_read("skybox.bmp");
	level->spray = bmp_read("spray.bmp");
	level->main_menu_title = bmp_read("embed/title.bmp");
	i = 0;
	while (i < VIEWMODEL_FRAMES)
	{
		viewmodel_name[19] = '0' + i;
		level->viewmodel[i] = bmp_read(viewmodel_name);
		i++;
	}
	level->visible.tris = (t_tri *)malloc(sizeof(t_tri)
			* level->all.tri_amount);
	if (!level->visible.tris)
		ft_error("memory allocation failed\n");
	return (level);
}

void	init_window(t_window **window)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
		ft_error("could not initialize SDL\n");
	*window = (t_window *)malloc(sizeof(t_window));
	if (!*window)
		ft_error("memory allocation failed\n");
	window[0]->SDLwindow = SDL_CreateWindow("DOOM NUKEM",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			RES_X, RES_Y, 0);
	if (!window[0]->SDLwindow)
		ft_error("could not create window");
	window[0]->SDLrenderer
		= SDL_CreateRenderer(window[0]->SDLwindow, -1, 0);
	if (!window[0]->SDLrenderer)
		ft_error("could not create renderer");
	window[0]->raster_texture = empty_texture(window[0]->SDLrenderer);
	SDL_SetTextureBlendMode(window[0]->raster_texture, SDL_BLENDMODE_BLEND);
	window[0]->texture = empty_texture(window[0]->SDLrenderer);
	window[0]->frame_buffer = NULL;
	window[0]->depth_buffer
		= (float *)malloc(sizeof(float) * (RES_X * RES_Y));
	if (!window[0]->depth_buffer)
		ft_error("memory allocation failed\n");
}

void	init_enemy(t_tri *face)
{
	face->isenemy = 1;
	face->enemy = (t_enemy *)malloc(sizeof(t_enemy));
	if (!face->enemy)
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

void	init_player(t_enemy *player)
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
	l->audio.music = Mix_LoadMUS(AUDIO_MUSIC);
	if (!l->audio.music)
		ft_error(AUDIO_MUSIC);
	l->audio.jump = Mix_LoadWAV(AUDIO_JUMP);
	if (!l->audio.jump)
		ft_error(AUDIO_JUMP);
	l->audio.gunshot = Mix_LoadWAV(AUDIO_GUNSHOT);
	if (!l->audio.gunshot)
		ft_error(AUDIO_GUNSHOT);
	l->audio.reload = Mix_LoadWAV(AUDIO_RELOAD);
	if (!l->audio.reload)
		ft_error(AUDIO_RELOAD);
	l->audio.death = Mix_LoadWAV(AUDIO_DEATH);
	if (!l->audio.death)
		ft_error(AUDIO_DEATH);
	l->audio.door = Mix_LoadWAV(AUDIO_DOOR);
	if (!l->audio.door)
		ft_error(AUDIO_DOOR);
}
