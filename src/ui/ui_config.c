/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/27 01:03:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/03 06:26:30 by rpehkone         ###   ########.fr       */
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

static void	ui_confing_face_render_settings(t_tri *tri)
{
	char	buf[100];

	sprintf(buf, "opacity: %.0f%%",
		100 * tri->opacity);
	float_slider(&tri->opacity, buf, 0, 1);
	if (tri->opacity)
	{
		sprintf(buf, "refractive index: %.2f",
			tri->refractivity);
		float_slider(&tri->refractivity, buf, -1, 3);
	}
	if (button(&tri->isquad, "quad"))
		set_fourth_vertex(tri);
	button(&tri->isgrid, "grid");
	int_slider(&tri->shader, "shader", 0, 2);
	button(&tri->isbreakable, "breakable");
	button(&tri->isenemy, "enemy");
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
	if (call("remove faces", &remove_faces, level))
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
	float_slider(&tri->reflectivity, buf, 0, 1);
	ui_confing_face_render_settings(tri);
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
			if (call(name, NULL, level))
				make_fileopen_call(level, name);
	}
	else if (isdir && level->ui.state.find_dir && call(name, NULL, level))
		go_in_dir(level->ui.state.directory, name);
}

void	ui_render_directory_header(t_level *level)
{
	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	text(level->ui.state.directory);
	if (call("close", NULL, level))
	{
		if (level->ui.state.open_file == &open_level)
			level->ui.main_menu = MAIN_MENU_LOCATION_MAIN;
		else
			level->ui.state.ui_location = UI_LOCATION_MAIN;
	}
	if (call("up dir ..", NULL, level))
		path_up_dir(level->ui.state.directory);
}

void	ui_render_directory(t_level *level)
{
	ui_render_directory_header(level);
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	level->ui.state.find_dir = TRUE;
	level->ui.state.find_extension = FALSE;
	loop_directory(level->ui.state.directory, (void *)level,
		&ui_loop_directory_callback);
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	level->ui.state.find_dir = FALSE;
	loop_directory(level->ui.state.directory, (void *)level,
		&ui_loop_directory_callback);
	set_text_color(UI_INFO_TEXT_COLOR);
	level->ui.state.find_extension = TRUE;
	loop_directory(level->ui.state.directory, (void *)level,
		&ui_loop_directory_callback);
	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	if (level->ui.state.ui_location == UI_LOCATION_FILE_SAVE)
	{
		text_input(level->ui.state.save_filename, level);
		if (call("save", NULL, level))
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

void	ui_render_info(t_level *level)
{
	char		buf[100];
	t_editor_ui	*ui;

	ui = &level->ui;
	set_text_color(UI_INFO_TEXT_COLOR);
	sprintf(buf, "fps:               %d", get_fps());
	text(buf);
	sprintf(buf, "cull:              %ums", ui->cull_time);
	text(buf);
	sprintf(buf, "ssp:               %ums", ui->ssp_time);
	text(buf);
	sprintf(buf, "render:          %ums", ui->render_time);
	text(buf);
	sprintf(buf, "frametime: %ums", ui->frame_time);
	text(buf);
	sprintf(buf, "faces:           %d / %d",
		level->all.tri_amount, level->visible.tri_amount);
	text(buf);
	sprintf(buf, "xz velocity:  %.2fm/s", level->ui.horizontal_velocity);
	text(buf);
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
	int_slider(&ui->chromatic_abberation, "chroma", 0, 30);
	fov_angle = ui->fov + 0.01;
	fov_angle *= 180.0 / M_PI;
	sprintf(buf, "fov: %d", (int)fov_angle);
	float_slider(&ui->fov, buf, M_PI / 6, M_PI);
	button(&ui->blur, "blur");
	button(&ui->smooth_pixels, "smooth pixel transition");
	button(&ui->state.ssp_visual, "ssp visualize");
}

void	ui_settings(t_level *level)
{
	char		buf[100];

	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (call("close", NULL, level))
		level->ui.main_menu = MAIN_MENU_LOCATION_MAIN;
	if (call("select spray", NULL, level))
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

	call("set door start position", &set_door_pos_1, level);
	call("set door stop position", &set_door_pos_2, level);
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
		if (call("move door activation button", NULL, level))
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

	if (call("close door editor", NULL, level))
		level->ui.state.ui_location = UI_LOCATION_MAIN;
	find_selected_door_index(level);
	if (level->doors.selected_index)
		call("delete selected door", &delete_door, level);
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
			call("new door from selection", &add_new_door, level);
		else
			text("Select faces to create door from");
	}
}

void	ui_single_light_settings(t_level *level)
{
	char	buf[100];
	int		changed;

	changed = 0;
	changed += color_slider(&level->lights[level->selected_light_index - 1].color,
		"light color");
	sprintf(buf, "radius: %.2f",
		level->lights[level->selected_light_index - 1].radius);
	changed += float_slider(&level->lights[level->selected_light_index - 1].radius,
		buf, .1, 20);
	sprintf(buf, "power: %.2f",
		level->lights[level->selected_light_index - 1].power);
	changed += float_slider(&level->lights[level->selected_light_index - 1].power,
		buf, .1, 5);
	call("delete light", &delete_light, level);
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
		call(buf, start_bake, level);
	}
	else if (level->bake_status == BAKE_BAKED)
	{
		set_text_color(UI_LEVEL_BAKED_COLOR);
		sprintf(buf, "lighting baked");
		if (call(buf, NULL, level))
			level->bake_status = BAKE_NOT_BAKED;
	}
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (call("close light editor", NULL, level))
		level->ui.state.ui_location = UI_LOCATION_MAIN;
	ui_level_light_settings(level);
	call("add light", &add_light, level);
	if (level->selected_light_index)
		ui_single_light_settings(level);
}

void	ui_level_set_var(t_level *level)
{
	char	buf[100];

	sprintf(buf, "win distance: %.2fm", level->win_dist);
	float_slider(&level->win_dist, buf, 1, 40);
	call("set win position", &set_win_pos, level);
	call("set spawn position", &set_spawn_pos, level);
	call("set menu position 1", &set_menu_pos_1, level);
	call("set menu position 2", &set_menu_pos_2, level);
	sprintf(buf, "main menu animation time %ds",
		level->main_menu_anim_time);
	int_slider((int *)&level->main_menu_anim_time, buf, 2, 50);
}

void	ui_level_settings(t_level *level)
{
	char	buf[100];

	call("edit lights", &enable_light_editor, level);
	file_browser("select obj", ".obj", &set_obj);
	file_browser("select texture", ".bmp", &set_texture);
	file_browser("select normal map", ".bmp", &set_normal_map);
	file_browser("select skybox", ".bmp", &set_skybox);
	call("add face", &add_face, level);
	ui_level_set_var(level);
	float_slider(&level->player.projectile_scale,
		"Player projectile scale: ", 0, 1.5);
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
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	call("edit uv", &enable_uv_editor, level);
	call("edit doors", &enable_door_editor, level);
	if (nothing_selected(level) && level->bake_status != BAKE_BAKING)
	{
		ui_level_settings(level);
		ui_render_info(level);
	}
	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	ui_config_selected_faces(level);
}

void	ui_baking(t_level *level)
{
	char	buf[100];

	button(&level->ui.noclip, "noclip");
	button(&level->ui.wireframe, "wireframe");
	sprintf(buf, "baking: %.3f%%", level->bake_progress);
	set_text_color(UI_LEVEL_BAKING_COLOR);
	if (call(buf, NULL, level))
		level->bake_status = BAKE_NOT_BAKED;
}

void	select_editor_ui(t_level *level)
{
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	if (level->bake_status == BAKE_BAKING)
		ui_baking(level);
	else if (level->ui.state.ui_location == UI_LOCATION_FILE_SAVE
		|| level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
		ui_render_directory(level);
	else if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
	{
		if (call("close uv editor", NULL, level))
			level->ui.state.ui_location = UI_LOCATION_MAIN;
		call("fix selected uv overlap", &fix_uv_overlap, level);
	}
	else if (level->ui.state.ui_location == UI_LOCATION_DOOR_EDITOR)
		ui_door_editor(level);
	else if (level->ui.state.ui_location == UI_LOCATION_DOOR_ACTIVATION_BUTTON)
	{
		if (call("return to door editor", NULL, level))
			level->ui.state.ui_location = UI_LOCATION_DOOR_EDITOR;
	}
	else if (level->ui.state.ui_location == UI_LOCATION_LIGHT_EDITOR)
		ui_light_editor(level);
	else
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
		ui_render_info(level);
	}
	else
		ui_render_directory(level);
}

void	ui(t_window *window, t_level *level, t_game_state *game_state)
{
	level->ui.state.ui_max_width = 0;
	level->ui.state.ui_text_color = 0;
	level->ui.state.ui_text_x_offset = 0;
	level->ui.state.ui_text_y_pos = 0;
	level->ui.state.current_font = level->ui.editor_font;
	if (level->ui.state.ssp_visual)
	{
		render_ssp_visual_background(window->ui_texture_pixels);
		render_ssp_visual_text(level);
	}
	if (*game_state == GAME_STATE_EDITOR)
	{
		if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
			uv_editor(level, window->ui_texture_pixels);
		select_editor_ui(level);
		door_put_text(level);
		light_put_text(level);
	}
	else if (*game_state == GAME_STATE_MAIN_MENU)
		ui_main_menu(window, level, game_state);
	else
		hud(level, window->ui_texture_pixels, *game_state);
	ui_render_background(window, level);
	ui_render_nonfatal_errors(level);
}
