/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/11/11 15:04:39 by rpehkone         ###   ########.fr       */
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

void	init_fonts(t_editor_ui *ui)
{
	SDL_RWops		*rwmem;
	unsigned char	*mem;
	unsigned long	size;

	TTF_Init();
	mem = read_embedded_file("digital.ttf");
	size = get_embedded_size("digital.ttf") - 1;
	ui->digital_data = mem;
	rwmem = SDL_RWFromMem((void *)mem, size);
	ui->hud_font = TTF_OpenFontRW(rwmem, 1, HUD_FONT_SIZE);
	mem = read_embedded_file("roboto.ttf");
	size = get_embedded_size("roboto.ttf") - 1;
	ui->roboto_data = mem;
	rwmem = SDL_RWFromMem((void *)mem, size);
	ui->editor_font = TTF_OpenFontRW(rwmem, 1, UI_FONT_SIZE);
	rwmem = SDL_RWFromMem((void *)mem, size);
	ui->main_menu_font = TTF_OpenFontRW(rwmem, 1, MAIN_MENU_FONT_SIZE);
	rwmem = SDL_RWFromMem((void *)mem, size);
	ui->win_lose_font = TTF_OpenFontRW(rwmem, 1, HUD_GAME_EVENT_FONT_SIZE);
	if (!ui->editor_font || !ui->hud_font
		|| !ui->main_menu_font || !ui->win_lose_font)
		ft_error("font open fail");
}

void	init_animation(t_level *level)
{
	unsigned char	*mem;
	int				res;

	res = 0;
	mem = read_embedded_file("enemy_shoot.obj");
	load_obj_from_memory(mem, &level->game_models.enemy_shoot);
	free(mem);
	res += load_animation("_run",
			&level->game_models.enemy_run, 10, 1.0);
	level->game_models.enemy
		= get_animation_target(&level->game_models.enemy_run);
	res += load_animation("_die",
			&level->game_models.enemy_die, 14, 3.0);
	level->game_models.enemy_die.loop = FALSE;
	res += load_animation("odel", &level->game_models.reload_animation,
			11, RELOAD_ANIMATION_DURATION);
	level->game_models.viewmodel
		= get_animation_target(&level->game_models.reload_animation);
	if (res != 3)
		ft_error("animation read fail");
}

void	init_textures_2(t_level *level)
{
	unsigned char	*mem;

	mem = read_embedded_file("vulcan_diff.bmp");
	level->game_models.viewmodel.texture
		= bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("lightsprite.bmp");
	level->game_models.light_sprite
		= bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("projectile.bmp");
	level->game_models.projectile_sprite
		= bmp_read_from_memory(mem);
	free(mem);
}

void	init_textures(t_level *level)
{
	unsigned char	*mem;

	mem = read_embedded_file("title.bmp");
	level->main_menu_title = bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("ammo_diff.bmp");
	level->game_models.ammo_pickup_box.texture
		= bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("health_diff.bmp");
	level->game_models.health_pickup_box.texture
		= bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("enemy_diff.bmp");
	level->game_models.enemy.texture
		= bmp_read_from_memory(mem);
	free(mem);
	init_textures_2(level);
}

void	projectile_default(t_projectile *projectile)
{
	projectile->speed = 10;
	projectile->dist = 1;
	projectile->damage = 10;
}

void	init_enemy_settings(t_enemy_settings *enemy)
{
	enemy->dist_limit = 30;
	enemy->move_speed = 2;
	enemy->initial_health = 100;
	enemy->melee_range = 1.5;
	enemy->attack_frequency = 500.0;
	enemy->move_duration = 3.0;
	enemy->shoot_duration = 2.0;
	enemy->melee_damage = 30;
}

void	level_default_settings(t_level *level)
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
	level->ui.sun_color.hue = 0.768421;
	level->ui.sun_color.saturation = 0.0;
	level->ui.sun_color.lightness = 0.936842;
	hsl_update_color(&level->ui.sun_color);
	level->game_logic.win_pos.x = 10;
	init_enemy_settings(&level->game_logic.enemy_settings);
	projectile_default(&level->game_logic.enemy_projectile_settings);
	level->game_logic.enemy_projectile_settings.shot_by_player = FALSE;
	projectile_default(&level->game_logic.player_projectile_settings);
	level->game_logic.player_projectile_settings.shot_by_player = TRUE;
}

void	read_embedded_images(t_level *level, unsigned char *mem)
{
	mem = read_embedded_file("normal.bmp");
	if (level->normal_map.image)
		free(level->normal_map.image);
	level->normal_map = bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("skybox.bmp");
	if (level->sky.img.image)
		free(level->sky.img.image);
	level->sky.img = bmp_read_from_memory(mem);
	free(mem);
	mem = read_embedded_file("spray.bmp");
	if (level->spray.image)
		free(level->spray.image);
	level->spray = bmp_read_from_memory(mem);
	free(mem);
}

void	create_default_level(t_level *level)
{
	unsigned char	*mem;

	delete_all_doors(level);
	delete_all_lights(level);
	level_default_settings(level);
	mem = read_embedded_file("out.bmp");
	if (level->texture.image)
		free(level->texture.image);
	level->texture = bmp_read_from_memory(mem);
	free(mem);
	realloc_baked_and_spray(level);
	read_embedded_images(level, mem);
	mem = read_embedded_file("ship_final.obj");
	if (level->all.tris)
	{
		free_culling(level);
		free(level->all.tris);
	}
	load_obj_from_memory(mem, &level->all);
	free(mem);
	init_culling(level);
}

void	init_window_struct(t_window **window)
{
	window[0]->sdl_window = SDL_CreateWindow("DOOM NUKEM",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			RES_X, RES_Y, SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window[0]->sdl_window)
		ft_error("could not create window");
	window[0]->sdl_renderer
		= SDL_CreateRenderer(window[0]->sdl_window, -1, 0);
	if (!window[0]->sdl_renderer)
		ft_error("could not create renderer");
	window[0]->raster_texture = empty_texture(window[0]->sdl_renderer);
	SDL_SetTextureBlendMode(window[0]->raster_texture, SDL_BLENDMODE_BLEND);
	window[0]->texture = empty_texture(window[0]->sdl_renderer);
	window[0]->frame_buffer = NULL;
	window[0]->depth_buffer
		= (float *)malloc(sizeof(float) * (RES_X * RES_Y));
	window[0]->pixel_pos_buffer
		= (t_vec3 *)malloc(sizeof(t_vec3) * (RES_X * RES_Y));
	window[0]->brightness_buffer
		= (t_color *)malloc(sizeof(t_color) * (RES_X * RES_Y));
	memset((void *)window[0]->brightness_buffer, 0,
		sizeof(t_color) * (RES_X * RES_Y));
	window[0]->post_process_buf
		= (unsigned int *)malloc(sizeof(unsigned int) * (RES_X * RES_Y));
	check_buffers(*window);
}

void	init_window(t_window **window)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
		ft_error("could not initialize SDL\n");
	*window = (t_window *)malloc(sizeof(t_window));
	if (!*window)
		ft_error("memory allocation failed\n");
	init_window_struct(window);
	SDL_RenderClear((*window)->sdl_renderer);
}

void	init_audio_effects_2(t_level *l)
{
	unsigned char	*mem;
	SDL_RWops		*rw;

	mem = read_embedded_file("death.ogg");
	rw = SDL_RWFromMem(mem,
			(int)get_embedded_size("death.ogg") - 1);
	l->audio.death_mem = mem;
	l->audio.death = Mix_LoadWAV_RW(rw, 1);
	mem = read_embedded_file("jump.wav");
	rw = SDL_RWFromMem(mem,
			(int)get_embedded_size("jump.wav") - 1);
	l->audio.jump_mem = mem;
	l->audio.jump = Mix_LoadWAV_RW(rw, 1);
	mem = read_embedded_file("gunshot.wav");
	rw = SDL_RWFromMem(mem,
			(int)get_embedded_size("gunshot.wav") - 1);
	l->audio.gunshot_mem = mem;
	l->audio.gunshot = Mix_LoadWAV_RW(rw, 1);
}

void	init_audio_effects(t_level *l)
{
	unsigned char	*mem;
	SDL_RWops		*rw;

	init_audio_effects_2(l);
	mem = read_embedded_file("reload.wav");
	rw = SDL_RWFromMem(mem,
			(int)get_embedded_size("reload.wav") - 1);
	l->audio.reload_mem = mem;
	l->audio.reload = Mix_LoadWAV_RW(rw, 1);
	mem = read_embedded_file("door.wav");
	rw = SDL_RWFromMem(mem,
			(int)get_embedded_size("door.wav") - 1);
	l->audio.door_mem = mem;
	l->audio.door = Mix_LoadWAV_RW(rw, 1);
	if (!l->audio.reload || !l->audio.gunshot || !l->audio.jump
		|| !l->audio.death || !l->audio.door)
		ft_error("audio effects init error");
}

void	init_audio(t_level *l)
{
	unsigned char	*mem;
	SDL_RWops		*rw;

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096) < 0)
	{
		nonfatal_error("Failed to initialize audio");
		return ;
	}
	l->audio.music_volume = MIX_MAX_VOLUME / 10;
	l->audio.sound_effect_volume = MIX_MAX_VOLUME / 5;
	Mix_VolumeMusic(l->audio.music_volume);
	Mix_Volume(-1, l->audio.sound_effect_volume);
	mem = read_embedded_file("main_menu.ogg");
	rw = SDL_RWFromMem(mem, (int)get_embedded_size("main_menu.ogg") - 1);
	l->audio.title_music_mem = mem;
	l->audio.title_music = Mix_LoadMUS_RW(rw, 1);
	mem = read_embedded_file("ingame.ogg");
	rw = SDL_RWFromMem(mem, (int)get_embedded_size("ingame.ogg") - 1);
	l->audio.game_music_mem = mem;
	l->audio.game_music = Mix_LoadMUS_RW(rw, 1);
	if (!l->audio.game_music || !l->audio.title_music)
		ft_error("audio init error");
	init_audio_effects(l);
}

void	check_buffers(t_window *window)
{
	if (!window->depth_buffer
		|| !window->post_process_buf
		|| !window->pixel_pos_buffer)
		ft_error("init window memory allocation failed\n");
}

void	init_embedded(t_level *level)
{
	unsigned char	*mem;

	mem = read_embedded_file("skybox.obj");
	load_obj_from_memory(mem, &level->sky.all);
	free(mem);
	mem = read_embedded_file("skybox.obj");
	load_obj_from_memory(mem, &level->sky.visible);
	free(mem);
	mem = read_embedded_file("pickup_box.obj");
	load_obj_from_memory(mem, &level->game_models.ammo_pickup_box);
	free(mem);
	mem = read_embedded_file("pickup_box.obj");
	load_obj_from_memory(mem, &level->game_models.health_pickup_box);
	free(mem);
	init_animation(level);
	init_textures(level);
	init_fonts(&level->ui);
}

void	realloc_baked_and_spray(t_level *level)
{
	if (level->baked)
		free(level->baked);
	level->baked = (t_color *)malloc(sizeof(t_color)
			* (level->texture.width * level->texture.height));
	if (!level->baked)
		ft_error("memory allocation failed\n");
	if (level->spray_overlay)
		free(level->spray_overlay);
	level->spray_overlay = (unsigned int *)malloc(sizeof(unsigned int)
			* (level->texture.width * level->texture.height));
	if (!level->spray_overlay)
		ft_error("memory allocation failed\n");
	ft_bzero(level->spray_overlay,
		level->texture.width * level->texture.height * 4);
	level->bake_status = BAKE_NOT_BAKED;
}
