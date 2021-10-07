/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 14:38:45 by vkuikka           #+#    #+#             */
/*   Updated: 2021/09/17 22:52:45 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"
#include "embed.h"

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

static void	init_fonts(t_editor_ui *ui)
{
	SDL_RWops	*mem;

	TTF_Init();
	mem = SDL_RWFromMem((void *)&embed_digital_ttf[0],
			(int)embed_digital_ttf_len);
	ui->hud_font = TTF_OpenFontRW(mem, 1, HUD_FONT_SIZE);
	mem = SDL_RWFromMem((void *)&embed_Roboto_Medium_ttf[0],
			(int)embed_Roboto_Medium_ttf_len);
	ui->editor_font = TTF_OpenFontRW(mem, 1, UI_FONT_SIZE);
	mem = SDL_RWFromMem((void *)&embed_Roboto_Medium_ttf[0],
			(int)embed_Roboto_Medium_ttf_len);
	ui->main_menu_font = TTF_OpenFontRW(mem, 1, MAIN_MENU_FONT_SIZE);
	mem = SDL_RWFromMem((void *)&embed_Roboto_Medium_ttf[0],
			(int)embed_Roboto_Medium_ttf_len);
	ui->win_lose_font = TTF_OpenFontRW(mem, 1, HUD_GAME_EVENT_FONT_SIZE);
	if (!ui->editor_font || !ui->hud_font
		|| !ui->main_menu_font || !ui->win_lose_font)
	{
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		ft_error("font open fail");
	}
}

void	init_embedded(t_level *level)
{
	level->main_menu_title
		= bmp_read_from_memory(&embed_title_bmp[0], embed_title_bmp_len);
	load_obj_from_memory(&embed_skybox_obj[0], embed_skybox_obj_len,
		&level->sky.all);
	load_obj_from_memory(&embed_skybox_obj[0], embed_skybox_obj_len,
		&level->sky.visible);
	init_fonts(&level->ui);
	load_obj_from_memory(&embed_pickup_box_obj[0], embed_pickup_box_obj_len,
		&level->game_models.ammo_pickup_box);
	load_obj_from_memory(&embed_pickup_box_obj[0], embed_pickup_box_obj_len,
		&level->game_models.health_pickup_box);
	int res = 0;
	res += load_obj("embed/enemy_shoot.obj", &level->game_models.enemy_shoot);
	res += load_animation("embed/enemy_run/enemy_run", &level->game_models.enemy_run, 10, 1.0);
	level->game_models.enemy = get_animation_target(&level->game_models.enemy_run);
	res += load_animation("embed/enemy_die/enemy_die", &level->game_models.enemy_die, 41, 3.0);
	level->game_models.enemy_die.loop = FALSE;
	level->game_models.enemy = get_animation_target(&level->game_models.enemy_die);
	res += load_animation("embed/viewmodel/viewmodel", &level->game_models.reload_animation, 3,
			RELOAD_ANIMATION_DURATION);
	level->game_models.viewmodel = get_animation_target(&level->game_models.reload_animation);
	obj_copy(&level->game_models.enemy, &level->game_models.enemy_shoot);
	if (res != 4)
		ft_error("fix embed\n");
	level->game_models.ammo_pickup_box.texture
		= bmp_read_from_memory(&embed_ammo_pickup_texture_bmp[0], embed_ammo_pickup_texture_bmp_len);
	level->game_models.health_pickup_box.texture
		= bmp_read_from_memory(&embed_health_pickup_texture_bmp[0], embed_health_pickup_texture_bmp_len);
	level->game_models.enemy.texture = bmp_read("embed/enemy_texture.bmp");
	level->game_models.viewmodel.texture = bmp_read("embed/viewmodel/viewmodel_texture.bmp");
	level->game_models.light_sprite = bmp_read("embed/light_sprite.bmp");
	level->game_models.projectile_sprite = bmp_read("embed/projectile_sprite.bmp");
	// level->game_models.light_sprite = bmp_read("embed/spray.bmp");
}

static void	projectile_default(t_projectile *projectile)
{
	projectile->speed = 10;
	projectile->dist = 1;
	projectile->damage = 10;
}

static void	init_enemy_settings(t_enemy_settings *enemy)
{
	enemy->dist_limit = 1;
	enemy->move_speed = 2;
	enemy->initial_health = 100;
	enemy->attack_range = 1.5;
	enemy->attack_frequency = 0.5;
}

static void	level_default_settings(t_level *level)
{
	level->game_logic.player.health = PLAYER_HEALTH_MAX;
	level->game_logic.player.ammo = PLAYER_AMMO_MAX;
	level->game_logic.win_dist = INITIAL_LEVEL_WIN_DIST;
	level->cam.pos.x = 0;
	level->cam.pos.y = -PLAYER_EYE_HEIGHT;
	level->cam.pos.z = 0;
	level->cam.look_side = 0;
	level->cam.look_up = 0;
	level->main_menu_anim.duration = 2;
	level->world_brightness = 0.5;
	level->game_logic.win_pos.x = 10;
	init_enemy_settings(&level->game_logic.enemy_settings);
	projectile_default(&level->game_logic.enemy_projectile_settings);
	projectile_default(&level->game_logic.player_projectile_settings);
}

void	create_default_level(t_level *level)
{
	level_default_settings(level);
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
	level->sky.img = bmp_read("skybox.bmp");
	level->spray = bmp_read("embed/spray.bmp");
	load_obj("level/cache.obj", &level->all);
	level->visible.tris
		= (t_tri *)malloc(sizeof(t_tri) * level->all.tri_amount);
	if (!level->visible.tris)
		ft_error("memory allocation failed\n");
	init_screen_space_partition(level);
	init_culling(level);
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
			RES_X, RES_Y, SDL_WINDOW_ALLOW_HIGHDPI);
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
	window[0]->post_process_buf
		= (unsigned int *)malloc(sizeof(unsigned int) * (RES_X * RES_Y));
	if (!window[0]->depth_buffer || !window[0]->post_process_buf)
		ft_error("init window memory allocation failed\n");
}

static void	init_audio_effects(t_level *l)
{
	SDL_RWops	*rw;

	rw = SDL_RWFromMem(embed_audio_effects_d_death_ogg,
			embed_audio_effects_d_death_ogg_len);
	l->audio.death = Mix_LoadWAV_RW(rw, 1);
	rw = SDL_RWFromMem(embed_audio_effects_jump_wav,
			embed_audio_effects_jump_wav_len);
	l->audio.jump = Mix_LoadWAV_RW(rw, 1);
	rw = SDL_RWFromMem(embed_audio_effects_gunshot_wav,
			embed_audio_effects_gunshot_wav_len);
	l->audio.gunshot = Mix_LoadWAV_RW(rw, 1);
	rw = SDL_RWFromMem(embed_audio_effects_reload_wav,
			embed_audio_effects_reload_wav_len);
	l->audio.reload = Mix_LoadWAV_RW(rw, 1);
	rw = SDL_RWFromMem(embed_audio_effects_door_wav,
			embed_audio_effects_door_wav_len);
	l->audio.door = Mix_LoadWAV_RW(rw, 1);
	if (!l->audio.reload || !l->audio.gunshot || !l->audio.jump
		|| !l->audio.death || !l->audio.door)
		ft_error("audio effects init error");
}

void	init_audio(t_level *l)
{
	SDL_RWops	*rw;

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096) < 0)
	{
		nonfatal_error("Failed to initialize audio");
		return ;
	}
	l->audio.music_volume = MIX_MAX_VOLUME / 10;
	l->audio.sound_effect_volume = MIX_MAX_VOLUME / 10;
	Mix_VolumeMusic(l->audio.music_volume);
	Mix_Volume(-1, l->audio.sound_effect_volume);
	rw = SDL_RWFromMem(embed_audio_music_main_menu_ogg,
			embed_audio_music_main_menu_ogg_len);
	l->audio.title_music = Mix_LoadMUS_RW(rw, 1);
	rw = SDL_RWFromMem(embed_audio_music_ingame_ogg,
			embed_audio_music_ingame_ogg_len);
	l->audio.game_music = Mix_LoadMUS_RW(rw, 1);
	if (!l->audio.game_music || !l->audio.title_music)
		ft_error("audio init error");
	init_audio_effects(l);
}
