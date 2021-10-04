/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/27 01:03:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/10/04 21:08:31 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

int	get_fps(void)
{
	struct timeval	time;
	static long		s;
	static int		frames;
	static int		fps;

	frames++;
	gettimeofday(&time, NULL);
	if (s != time.tv_sec)
	{
		s = time.tv_sec;
		fps = frames;
		frames = 0;
	}
	return (fps);
}

int	get_selected_amount(t_level *level)
{
	int	selected_amount;
	int	i;

	i = 0;
	selected_amount = 0;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
			selected_amount++;
		i++;
	}
	return (selected_amount);
}

void	copy_tri_settings(t_tri *a, t_tri *b)
{
	a->isbreakable = b->isbreakable;
	a->isenemy = b->isenemy;
	a->isgrid = b->isgrid;
	a->opacity = b->opacity;
	a->opacity_precise = b->opacity_precise;
	a->reflectivity = b->reflectivity;
	a->refractivity = b->refractivity;
	a->disable_distance_culling = b->disable_distance_culling;
	a->disable_backface_culling = b->disable_backface_culling;
	if (a->enemy && b->enemy)
	{
		a->enemy->attack_damage = b->enemy->attack_damage;
		a->enemy->attack_frequency = b->enemy->attack_frequency;
		a->enemy->attack_range = b->enemy->attack_range;
		a->enemy->dist_limit = b->enemy->dist_limit;
		a->enemy->initial_health = b->enemy->initial_health;
		a->enemy->move_speed = b->enemy->move_speed;
		a->enemy->projectile_speed = b->enemy->projectile_speed;
		a->enemy->projectile_scale = b->enemy->projectile_scale;
		a->enemy->projectile_uv[0] = b->enemy->projectile_uv[0];
		a->enemy->projectile_uv[1] = b->enemy->projectile_uv[1];
		a->enemy->projectile_uv[2] = b->enemy->projectile_uv[2];
	}
}

static void	ui_config_enemy_projectile_settings(t_tri *tri)
{
	char	buf[100];

	sprintf(buf,
		"projectile speed: %.1fm/s (0 = no projectile)",
		tri->enemy->projectile_speed);
	float_slider(&tri->enemy->projectile_speed,
		buf, 0, 50);
	sprintf(buf, "projectile scale: %.2f",
		tri->enemy->projectile_scale);
	float_slider(&tri->enemy->projectile_scale,
		buf, 0.1, 5);
}

static void	ui_config_enemy_settings(t_tri *tri)
{
	char	buf[100];

	if (!tri->enemy)
		init_enemy(tri);
	sprintf(buf, "distance limit: %.1fm",
		tri->enemy->dist_limit);
	float_slider(
		&tri->enemy->dist_limit, buf, 1, 10);
	sprintf(buf, "move speed: %.1fm/s",
		tri->enemy->move_speed);
	float_slider(
		&tri->enemy->move_speed, buf, 0, 10);
	sprintf(buf, "attack frequency: %.2f seconds per attack",
		tri->enemy->attack_frequency);
	float_slider(
		&tri->enemy->attack_frequency, buf, 0, 5);
	sprintf(buf, "attack damage: %.1f",
		tri->enemy->attack_damage);
	float_slider(
		&tri->enemy->attack_damage, buf, 0, 50);
	sprintf(buf, "attack range: %.1fm",
		tri->enemy->attack_range);
	float_slider(
		&tri->enemy->attack_range, buf, 0, 10);
	ui_config_enemy_projectile_settings(tri);
}

static void	ui_config_face_perlin_settings(t_perlin_settings *p)
{
	char	buf[100];

	float_slider(&p->min, "min", 0, 1);
	float_slider(&p->max, "max", 0, 1);
	float_slider(&p->noise_opacity, "opacity from noise", 0, 4);
	float_slider(&p->distance, "distance (0 = disabled)", 0, 800);
	int_slider(&p->resolution, "resolution", 1, 6);
	float_slider(&p->depth, "depth", 0, 5);
	float_slider(&p->scale, "scale", 0.01, 2);
	float_slider(&p->move_speed, "speed", 0, 3);
	sprintf(buf, "depth speed difference: %.2f", p->speed_diff);
	float_slider(&p->speed_diff, buf, 0, 4);
	int_slider(&p->visualizer, "visualizer", 0, 3);
	if (color_slider(&p->color_1, "color 1"))
		hsl_update_color(&p->color_1);
	if (color_slider(&p->color_2, "color 2"))
		hsl_update_color(&p->color_2);
}

static void	ui_config_face_perlin(t_perlin_settings *perlin, t_level *level)
{
	set_text_color(UI_SHADER_SETTINGS);
	float_slider(&perlin->swirl, "swirl", 0, 5);
	if (perlin->swirl != 0)
	{
		float_slider(&perlin->swirl_interval, "swirl interval", 0, 20);
		if (call("set swirl position", NULL))
		{
			perlin->dir.x = level->cam.pos.x;
			perlin->dir.y = level->cam.pos.z;
		}
	}
	else
	{
		if (call("set noise direction", NULL))
		{
			perlin->dir.x = level->cam.front.x;
			perlin->dir.y = level->cam.front.z;
		}
		vec2_normalize(&perlin->dir);
	}
	ui_config_face_perlin_settings(perlin);
}

static void	ui_confing_face_render_settings(t_tri *tri, t_level *level)
{
	char	buf[100];

	sprintf(buf, "opacity: %.0f%%",
		100 * tri->opacity);
	float_slider(&tri->opacity, buf, 0, 1);
	if (tri->opacity)
	{
		button(&tri->opacity_precise, "precise opacity");
		sprintf(buf, "refractive index: %.2f",
			tri->refractivity);
		float_slider(&tri->refractivity, buf, -1, 3);
	}
	if (button(&tri->isquad, "quad"))
		set_fourth_vertex(tri);
	button(&tri->isgrid, "grid");
	button(&tri->isbreakable, "breakable");
	button(&tri->isenemy, "enemy");
	if (call("shader editor", NULL))
		level->ui.state.ui_location = UI_LOCATION_SHADER_EDITOR;
}

static void	ui_confing_face_settings(t_level *level,
							int selected_amount, t_tri *tri)
{
	char	buf[100];

	if (selected_amount == 1)
		text("Selected face:");
	else
	{
		sprintf(buf, "%d faces selected:", selected_amount);
		text(buf);
	}
	if (call("remove faces", &remove_faces))
		return ;
	if (!tri->reflectivity || selected_amount != 1)
		sprintf(buf, "reflectivity: %.0f%%",
			100 * tri->reflectivity);
	else
		sprintf(buf,
			"reflectivity: %.0f%% (%d mirror %d first bounce)",
			100 * tri->reflectivity,
			tri->reflection_obj_all.tri_amount,
			tri->reflection_obj_first_bounce.tri_amount);
	if (float_slider(&tri->reflectivity, buf, 0, 1))
		static_culling(level);
	ui_confing_face_render_settings(tri, level);
	if (tri->isenemy)
		ui_config_enemy_settings(tri);
}

void	ui_config_selected_faces(t_level *level)
{
	int	selected_amount;
	int	selected_index;
	int	i;

	selected_index = 0;
	selected_amount = get_selected_amount(level);
	i = -1;
	while (++i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			if (!selected_index)
			{
				ui_confing_face_settings(level,
					selected_amount, &level->all.tris[i]);
				selected_index = i + 1;
				continue ;
			}
			if (level->all.tris[selected_index - 1].isenemy
				&& !level->all.tris[i].enemy)
				init_enemy(&level->all.tris[i]);
			copy_tri_settings(&level->all.tris[i],
				&level->all.tris[selected_index - 1]);
		}
	}
}

void	make_fileopen_call(t_level *level, char *file)
{
	char	absolute_filename[PATH_MAX + NAME_MAX];

	ft_strcpy(absolute_filename, level->ui.state.directory);
	go_in_dir(absolute_filename, file);
	level->ui.state.open_file(level, absolute_filename);
}

void	ui_loop_directory_callback(int isdir, char *name, void *data)
{
	t_level	*level;
	char	*exten;
	int		extension_match;

	level = (t_level *)data;
	if (!isdir && !level->ui.state.find_dir)
	{
		exten = level->ui.state.extension;
		extension_match = FALSE;
		if (ft_strlen(name) > ft_strlen(exten)
			&& !ft_strcmp(exten, &name[ft_strlen(name) - ft_strlen(exten)]))
			extension_match = TRUE;
		if (level->ui.state.find_extension != extension_match)
		{
			if (level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
			{
				if (call(name, NULL))
					make_fileopen_call(level, name);
			}
			else
				text(name);
		}
	}
	else if (isdir && level->ui.state.find_dir && call(name, NULL))
		go_in_dir(level->ui.state.directory, name);
}

void	ui_render_directory_header(t_level *level)
{
	char		buf[100];

	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	text(level->ui.state.directory);
	if (level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
	{
		sprintf(buf, "select  %s  file", level->ui.state.extension);
		text(buf);
	}
	else
		text("Save file");
	if (call("close", NULL))
	{
		if (level->ui.state.open_file == &open_level)
			level->ui.main_menu = MAIN_MENU_LOCATION_MAIN;
		else
			level->ui.state.ui_location = UI_LOCATION_MAIN;
	}
	if (call("up dir ..", NULL))
		path_up_dir(level->ui.state.directory);
}

void	ui_render_directory(t_level *level)
{
	ui_render_directory_header(level);
	set_text_color(UI_DIRECTORY_FOLDER_COLOR);
	level->ui.state.find_dir = TRUE;
	level->ui.state.find_extension = FALSE;
	loop_directory(level->ui.state.directory, (void *)level,
		&ui_loop_directory_callback);
	set_text_color(UI_DIRECTORY_FILE_WANTED_COLOR);
	level->ui.state.find_dir = FALSE;
	loop_directory(level->ui.state.directory, (void *)level,
		&ui_loop_directory_callback);
	set_text_color(UI_DIRECTORY_FILE_OTHER_COLOR);
	level->ui.state.find_extension = TRUE;
	loop_directory(level->ui.state.directory, (void *)level,
		&ui_loop_directory_callback);
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (level->ui.state.ui_location == UI_LOCATION_FILE_SAVE)
	{
		text("save as:");
		text_input(level->ui.state.save_filename, level);
		if (call("save", NULL))
		{
			save_level(level);
			level->ui.state.ui_location = UI_LOCATION_MAIN;
		}
	}
}

int	nothing_selected(t_level *level)
{
	int	i;

	i = 0;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
			return (0);
		i++;
	}
	return (1);
}

void	ui_render_info(t_editor_ui *ui, t_level *level)
{
	char		buf[100];

	sprintf(buf, "fps: %d", get_fps());
	text(buf);
	sprintf(buf, " |   cull: %ums", ui->cull_time);
	text(buf);
	sprintf(buf, " |   ssp: %ums", ui->ssp_time);
	text(buf);
	sprintf(buf, " |   |   raycast amount: %uk", ui->total_raycasts / 1000);
	text(buf);
	sprintf(buf, " |   |   raycast: %ums", ui->raycast_time);
	text(buf);
	sprintf(buf, " |   |   post:       %ums", ui->post_time);
	text(buf);
	sprintf(buf, " |   |   raster:   %ums", ui->raster_time);
	text(buf);
	sprintf(buf, " |   |   ui:           %ums", ui->ui_time);
	text(buf);
	sprintf(buf, " |   render:      %ums", ui->render_time);
	text(buf);
	sprintf(buf, "frame: %ums", ui->frame_time);
	text(buf);
	sprintf(buf, "faces: %d / %d",
		level->all.tri_amount, level->visible.tri_amount);
	text(buf);
}

static void	center_screen_print_line(t_vec2 dir, unsigned int color)
{
	t_window	*window;
	t_vec3		start;
	t_vec3		stop;

	window = get_window(NULL);
	start.x = RES_X / 2;
	start.y = RES_Y / 2;
	start.z = 0;
	stop.x = RES_X / 2 + (dir.x * 20);
	stop.y = RES_Y / 2 + (dir.y * -20);
	stop.z = 0;
	print_line(start, stop, color, window->ui_texture_pixels);
}

void	ui_physics_info(t_editor_ui *ui, t_level *level)
{
	char	buf[100];
	t_vec2	tmp;
	t_vec3	n;

	button(&ui->physics_debug, "physics debug");
	if (!ui->physics_debug)
		return ;
	tmp.x = level->player_vel.x;
	tmp.y = level->player_vel.z;
	center_screen_print_line(tmp, 0xff0000ff);
	sprintf(buf, "xz vel: %.2fm/s", level->ui.horizontal_velocity);
	set_text_color(0xff0000ff);
	render_text(buf, RES_X / 2, RES_Y / 2 + (UI_ELEMENT_HEIGHT * 1));
	center_screen_print_line(ui->wishdir, 0xff00ff);
	set_text_color(0xff00ff);
	render_text("wishdir", RES_X / 2, RES_Y / 2 + (UI_ELEMENT_HEIGHT * 2));
	n = level->cam.front;
	vec_normalize(&n);
	tmp.x = n.x;
	tmp.y = n.z;
	center_screen_print_line(tmp, 0xffff);
	set_text_color(0xffff);
	render_text("camera", RES_X / 2, RES_Y / 2 + (UI_ELEMENT_HEIGHT * 3));
}

void	ui_render_settings(t_level *level)
{
	char		buf[100];
	t_editor_ui	*ui;
	float		fov_angle;

	ui = &level->ui;
	sprintf(buf, "render scale: %d (%.0f%%)", ui->raycast_quality,
		100.0 / (float)ui->raycast_quality);
	int_slider(&ui->raycast_quality, buf, 1, 20);
	fov_angle = ui->fov + 0.01;
	fov_angle *= 180.0 / M_PI;
	sprintf(buf, "fov: %d", (int)fov_angle);
	float_slider(&ui->fov, buf, M_PI / 6, M_PI);
	int_slider(&ui->chromatic_abberation, "chroma (20ms expensive)", 0, 30);
	float_slider(&ui->sharpen, "sharpen (60ms very expensive)", 0.0, 5.0);
	button(&ui->smooth_pixels, "smooth pixel (20ms expensive)");
	button(&ui->blur, "blur (1ms cheap)");
	button(&ui->state.ssp_visual, "ssp visualize");
	sprintf(buf, "bloom radius: %.1f pixels", level->ui.bloom_radius);
	float_slider(&level->ui.bloom_radius, buf, 0, 100);
	sprintf(buf, "bloom intensity: %.1f", level->ui.bloom_intensity);
	float_slider(&level->ui.bloom_intensity, buf, 0, 5);
	sprintf(buf, "bloom limit: %.1f", level->ui.bloom_limit);
	float_slider(&level->ui.bloom_limit, buf, 0, 5);
	button(&level->ui.bloom_debug, "render bloom only");
}

void	ui_settings(t_level *level)
{
	char		buf[100];

	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (call("close", NULL))
		level->ui.main_menu = MAIN_MENU_LOCATION_MAIN;
	if (call("select spray", NULL))
	{
		level->ui.main_menu = MAIN_MENU_LOCATION_SPRAY_SELECT;
		ft_strcpy(level->ui.state.extension, ".bmp");
		level->ui.state.open_file = &set_spray;
	}
	button(&level->ui.spray_from_view, "spray from view");
	if (!level->ui.spray_from_view)
		sprintf(buf, "spray size: %.1f", level->ui.spray_size);
	if (!level->ui.spray_from_view)
		float_slider(&level->ui.spray_size, buf, 0.1, 5);
	ui_render_settings(level);
	sprintf(buf, "music volume: %.0f%%",
		100 * (level->audio.music_volume / MIX_MAX_VOLUME));
	float_slider(&level->audio.music_volume, buf, 0, MIX_MAX_VOLUME);
	Mix_VolumeMusic(level->audio.music_volume);
	sprintf(buf, "sound effect volume: %.0f%%",
		100 * (level->audio.sound_effect_volume / MIX_MAX_VOLUME));
	float_slider(&level->audio.sound_effect_volume, buf, 0, MIX_MAX_VOLUME);
	Mix_Volume(-1, level->audio.sound_effect_volume);
}

void	ui_door_settings(t_level *level)
{
	char	buf[100];

	call("set door start position", &set_door_pos_1);
	call("set door stop position", &set_door_pos_2);
	if (button(&level->doors.door[level->doors.selected_index - 1]
			.is_activation_pos_active, "has activation button"))
	{
		level->doors.door[level->doors.selected_index - 1]
			.activation_pos = level->cam.pos;
		vec_add(
			&level->doors.door[level->doors.selected_index - 1]
			.activation_pos,
			level->doors.door[level->doors.selected_index - 1]
			.activation_pos, level->cam.front);
	}
	if (level->doors.door[level->doors.selected_index - 1]
		.is_activation_pos_active)
		if (call("move door activation button", NULL))
			level->ui.state.ui_location
				= UI_LOCATION_DOOR_ACTIVATION_BUTTON;
	sprintf(buf, "door transition time: %fs",
		level->doors.door[level->doors.selected_index - 1]
		.transition_time);
	float_slider(&level->doors.door[level->doors.selected_index - 1]
		.transition_time, buf, .2, 7);
}

void	ui_door_editor(t_level *level)
{
	int			selected;
	int			i;

	if (call("close door editor", NULL))
		level->ui.state.ui_location = UI_LOCATION_MAIN;
	find_selected_door_index(level);
	if (level->doors.selected_index)
		call("delete selected door", &delete_door);
	if (level->doors.selected_index)
		ui_door_settings(level);
	else
	{
		selected = 0;
		i = -1;
		while (++i < level->all.tri_amount)
			if (level->all.tris[i].selected)
				selected++;
		if (selected)
			call("new door from selection", &add_new_door);
		else
			text("Select faces to create door from");
	}
}

void	ui_single_light_settings(t_level *level)
{
	char	buf[100];
	int		changed;

	if (!level->selected_light_index)
		return ;
	set_text_color(level->lights[level->selected_light_index - 1].color.color);
	changed = 0;
	changed += color_slider(
			&level->lights[level->selected_light_index - 1].color,
			"light color");
	sprintf(buf, "radius: %.2f",
		level->lights[level->selected_light_index - 1].radius);
	changed += float_slider(
			&level->lights[level->selected_light_index - 1].radius,
			buf, .1, 20);
	sprintf(buf, "power: %.2f",
		level->lights[level->selected_light_index - 1].power);
	changed += float_slider(
			&level->lights[level->selected_light_index - 1].power,
			buf, .1, 5);
	call("delete light", &delete_light);
	if (changed)
		level->bake_status = BAKE_NOT_BAKED;
}

void	ui_level_light_settings(t_level *level)
{
	char	buf[100];
	int		changed;

	changed = 0;
	sprintf(buf, "world brightness: %.2f", level->world_brightness);
	changed += float_slider(&level->world_brightness, buf, 0, 1);
	sprintf(buf, "skybox brightness: %.2f (0 = sync)",
		level->skybox_brightness);
	changed += float_slider(&level->skybox_brightness, buf, 0, 1);
	changed += color_slider(&level->ui.sun_color, "sun color");
	sprintf(buf, "sun dir: (%.2f, %.2f, %.2f)", level->ui.sun_dir.x,
		level->ui.sun_dir.y, level->ui.sun_dir.z);
	text(buf);
	changed += float_slider(&level->ui.sun_dir.x, NULL, -1, 1);
	changed += float_slider(&level->ui.sun_dir.y, NULL, -1, 1);
	changed += float_slider(&level->ui.sun_dir.z, NULL, -1, 1);
	if (changed)
	{
		vec_normalize(&level->ui.sun_dir);
		level->bake_status = BAKE_NOT_BAKED;
	}
}

void	ui_light_editor(t_level *level)
{
	char		buf[100];

	if (level->bake_status == BAKE_NOT_BAKED)
	{
		set_text_color(UI_LEVEL_NOT_BAKED_COLOR);
		sprintf(buf, "bake lighting");
		if (call(buf, start_bake))
			level->selected_light_index = 0;
	}
	else if (level->bake_status == BAKE_BAKED)
	{
		set_text_color(UI_LEVEL_BAKED_COLOR);
		sprintf(buf, "lighting baked");
		if (call(buf, NULL))
			level->bake_status = BAKE_NOT_BAKED;
	}
	set_text_color(level->ui.sun_color.color);
	if (call("close light editor", NULL))
	{
		level->ui.state.ui_location = UI_LOCATION_MAIN;
		level->selected_light_index = 0;
	}
	ui_level_light_settings(level);
	call("add light", &add_light);
	ui_single_light_settings(level);
}

void	ui_game_settings_delete_selected(t_level *level)
{
	if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_MENU_ANIMATION)
	{
		if (call("delete node", NULL))
			camera_path_delete_pos(&level->main_menu_anim,
				level->ui.state.logic_selected_index);
	}
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_AMMO)
		call("delete ammo box", &delete_ammo_box);
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_HEALTH)
		call("delete health box", &delete_health_box);
	else if (level->ui.state.logic_selected == GAME_LOGIC_SELECTED_ENEMY)
		call("delete enemy", &delete_enemy_spawn_pos);
}

void	ui_game_settings(t_level *level)
{
	char	buf[100];

	if (call("close", NULL))
		level->ui.state.ui_location = UI_LOCATION_MAIN;
	sprintf(buf, "win distance: %.2fm", level->game_logic.win_dist);
	float_slider(&level->game_logic.win_dist, buf, 1, 40);
	call("set win position", &set_win_pos);
	call("set spawn position", &set_spawn_pos);
	if (call("menu add camera pos", NULL))
		camera_path_add_pos(&level->main_menu_anim, level->cam);
	sprintf(buf, "main menu animation time %ds",
		level->main_menu_anim.duration);
	button(&level->main_menu_anim.loop, "main menu anim edge loop");
	int_slider((int *)&level->main_menu_anim.duration, buf, 2, 50);
	float_slider(&level->player.projectile_scale,
		"Player projectile scale: ", 0, 1.5);
	call("add enemy spawn", &add_enemy_spawn_pos);
	call("add ammo box", &add_ammo_box);
	call("add health box", &add_health_box);
	ui_game_settings_delete_selected(level);
}

void	choose_shader(t_tri *tri)
{
	text("shader:");
	if (call("none", NULL))
	{
		free(tri->perlin);
		tri->perlin = NULL;
		tri->shader = SHADER_NONE;
	}
	if (call("perlin", NULL))
	{
		if (!tri->perlin)
		{
			perlin_init(tri);
			noise2(0, 0);
		}
		tri->shader = SHADER_PERLIN;
	}
	if (call("rule 30", NULL))
		tri->shader = SHADER_RULE_30;
}

void	ui_shader_settings(t_level *level)
{
	int		i;
	t_tri	*tri;

	if (call("close", NULL))
		level->ui.state.ui_location = UI_LOCATION_MAIN;
	i = -1;
	while (++i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
		{
			tri = &level->all.tris[i];
			choose_shader(tri);
			if (tri->shader == SHADER_PERLIN && tri->perlin)
				ui_config_face_perlin(tri->perlin, level);
			return ;
		}
	}
	level->ui.state.ui_location = UI_LOCATION_MAIN;
}

void	ui_level_settings(t_level *level)
{
	char	buf[100];

	call("edit lights", &enable_light_editor);
	file_browser("select obj", ".obj", &set_obj);
	file_browser("select texture", ".bmp", &set_texture);
	file_browser("select skybox", ".bmp", &set_skybox);
	file_browser("select normal map", ".bmp", &set_normal_map);
	button(&level->ui.normal_map_disabled, "disable normal map");
	call("add face", &add_face);
	if (call("game settings", NULL))
	{
		level->ui.state.ui_location = UI_LOCATION_GAME_SETTINGS;
		level->ui.state.logic_selected = GAME_LOGIC_SELECTED_NONE;
	}
	button(&level->ui.fog, "fog");
	if (level->ui.fog)
		color_slider(&level->ui.fog_color, NULL);
	button(&level->ui.backface_culling, "backface culling");
	if (level->ui.backface_culling)
		button(&level->ui.occlusion_culling,
			"occlusion culling (O(n^2)) (Horrible trash)");
	button(&level->ui.distance_culling, "distance culling");
	sprintf(buf, "render distance: %.1fm", level->ui.render_distance);
	float_slider(&level->ui.render_distance, buf, 2, 50);
	file_save("save level", ".doom-nukem", NULL);
	sprintf(buf, "bloom radius: %.1f pixels", level->ui.bloom_radius);
	float_slider(&level->ui.bloom_radius, buf, 0, 100);
	sprintf(buf, "bloom intensity: %.1f", level->ui.bloom_intensity);
	float_slider(&level->ui.bloom_intensity, buf, 0, 5);
	sprintf(buf, "bloom limit: %.1f", level->ui.bloom_limit);
	float_slider(&level->ui.bloom_limit, buf, 0, 5);
	button(&level->ui.bloom_debug, "render bloom only");
}

void	ui_editor(t_level *level)
{
	t_editor_ui	*ui;

	ui = &level->ui;
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	button(&ui->noclip, "noclip");
	button(&ui->wireframe, "wireframe");
	if (ui->wireframe)
	{
		button(&ui->wireframe_on_top, "wireframe on top");
		button(&ui->show_quads, "quad visualize");
		button(&ui->wireframe_culling_visual, "culling visualize");
	}
	button(&ui->raytracing, "raytrace lights");
	button(&ui->vertex_select_mode, "vertex select mode");
	call("cull static", &static_culling);
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	call("edit uv", &enable_uv_editor);
	call("edit doors", &enable_door_editor);
	if (nothing_selected(level) && level->bake_status != BAKE_BAKING)
		ui_level_settings(level);
	set_text_color(WF_SELECTED_COL);
	ui_config_selected_faces(level);
	set_text_color(UI_INFO_TEXT_COLOR);
	ui_render_info(&level->ui, level);
	ui_physics_info(&level->ui, level);
}

void	ui_baking(t_level *level)
{
	char	buf[100];

	sprintf(buf, "baking: %.3f%%", level->bake_progress);
	set_text_color(UI_LEVEL_BAKING_COLOR);
	if (call(buf, NULL))
		level->bake_status = BAKE_NOT_BAKED;
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	button(&level->ui.noclip, "noclip");
	button(&level->ui.wireframe, "wireframe");
}

int	editor_select(t_level *level)
{
	if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
	{
		if (call("close uv editor", NULL))
			level->ui.state.ui_location = UI_LOCATION_MAIN;
		call("fix selected uv overlap", &fix_uv_overlap);
	}
	else if (level->ui.state.ui_location == UI_LOCATION_DOOR_EDITOR)
		ui_door_editor(level);
	else if (level->ui.state.ui_location == UI_LOCATION_DOOR_ACTIVATION_BUTTON)
	{
		if (call("return to door editor", NULL))
			level->ui.state.ui_location = UI_LOCATION_DOOR_EDITOR;
	}
	else if (level->ui.state.ui_location == UI_LOCATION_LIGHT_EDITOR)
		ui_light_editor(level);
	else if (level->ui.state.ui_location == UI_LOCATION_GAME_SETTINGS)
		ui_game_settings(level);
	else if (level->ui.state.ui_location == UI_LOCATION_SHADER_EDITOR)
		ui_shader_settings(level);
	else
		return (TRUE);
	return (FALSE);
}

void	select_editor_ui(t_level *level)
{
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (level->bake_status == BAKE_BAKING)
		ui_baking(level);
	else if (level->ui.state.ui_location == UI_LOCATION_FILE_SAVE
		|| level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
		ui_render_directory(level);
	else if (editor_select(level))
		ui_editor(level);
}

void	ui_main_menu(t_window *window, t_level *level, t_game_state *game_state)
{
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (level->ui.main_menu == MAIN_MENU_LOCATION_MAIN)
		main_menu(level, window->ui_texture_pixels, game_state);
	else if (level->ui.main_menu == MAIN_MENU_LOCATION_SETTINGS)
	{
		ui_settings(level);
		set_text_color(UI_INFO_TEXT_COLOR);
		ui_render_info(&level->ui, level);
	}
	else
		ui_render_directory(level);
}

static void	reset_ui_state(t_ui_state *state, t_level *level)
{
	state->ui_max_width = 0;
	state->ui_text_color = 0;
	state->ui_text_x_offset = 0;
	state->ui_text_y_pos = 0;
	state->current_font = level->ui.editor_font;
}

void	ui(t_window *window, t_level *level, t_game_state *game_state)
{
	reset_ui_state(&level->ui.state, level);
	if (level->ui.state.ssp_visual)
	{
		render_ssp_visual_background(window->ui_texture_pixels);
		render_ssp_visual_text(level);
	}
	if (*game_state == GAME_STATE_EDITOR)
	{
		if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
			uv_editor(level, window->ui_texture_pixels);
		else if (level->ui.state.ui_location == UI_LOCATION_GAME_SETTINGS)
			game_logic_put_info(level, window->ui_texture_pixels);
		else
		{
			door_put_text(level);
			light_put_text(level);
		}
		select_editor_ui(level);
	}
	else if (*game_state == GAME_STATE_MAIN_MENU)
		ui_main_menu(window, level, game_state);
	else
		hud(level, window->ui_texture_pixels, *game_state);
	ui_render_background(window, level);
	ui_render_nonfatal_errors(level);
}
