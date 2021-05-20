/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/27 01:03:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/20 16:10:07 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

int		    get_fps(void)
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

int		get_selected_amount(t_level *level)
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
		a->enemy->projectile_uv[0] = b->enemy->projectile_uv[0];
		a->enemy->projectile_uv[1] = b->enemy->projectile_uv[1];
		a->enemy->projectile_uv[2] = b->enemy->projectile_uv[2];
	}
}

void	ui_config_selected_faces(t_level *level)
{
	char buf[100];
	int	selected_amount;
	int selected_index;

	selected_index = 0;
	selected_amount = get_selected_amount(level);
	if (selected_amount)
		for (int i = 0; i < level->all.tri_amount; i++)
			reflection_culling(level, i);
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].selected)
		{
			if (!selected_index)
			{
				if (selected_amount == 1)
					text("Selected face:");
				else
				{
					sprintf(buf, "%d faces selected:", selected_amount);
					text(buf);
				}
				if (call("remove faces", &remove_faces, level))
					return;
				if (!level->all.tris[i].reflectivity || selected_amount != 1)
					sprintf(buf, "reflectivity: %.0f%%", 100 * level->all.tris[i].reflectivity);
				else
					sprintf(buf, "reflectivity: %.0f%% (%d mirror %d first bounce)", 100 * level->all.tris[i].reflectivity,
						level->all.tris[i].reflection_obj_all->tri_amount, level->all.tris[i].reflection_obj_first_bounce->tri_amount);
				float_slider(&level->all.tris[i].reflectivity, buf, 0, 1);
				sprintf(buf, "opacity: %.0f%%", 100 * level->all.tris[i].opacity);
				float_slider(&level->all.tris[i].opacity, buf, 0, 1);
				if (level->all.tris[i].opacity)
				{
					sprintf(buf, "refractive index: %.2f", level->all.tris[i].refractivity);
					float_slider(&level->all.tris[i].refractivity, buf, -1, 3);
				}
				if (button(&level->all.tris[i].isquad, "quad"))
					set_fourth_vertex(&level->all.tris[i]);
				button(&level->all.tris[i].isgrid, "grid");
				button(&level->all.tris[i].shader, "water");
				button(&level->all.tris[i].isenemy, "enemy");
				if (level->all.tris[i].isenemy)
				{
					if (!level->all.tris[i].enemy)
						init_enemy(&level->all.tris[i]);
					sprintf(buf, "distance limit: %.1fm", level->all.tris[i].enemy->dist_limit);
					float_slider(&level->all.tris[i].enemy->dist_limit, buf, 1, 10);
					sprintf(buf, "move speed: %.1fm/s", level->all.tris[i].enemy->move_speed);
					float_slider(&level->all.tris[i].enemy->move_speed, buf, 0, 10);
					sprintf(buf, "attack frequency: %.2f seconds per attack", level->all.tris[i].enemy->attack_frequency);
					float_slider(&level->all.tris[i].enemy->attack_frequency, buf, 0, 5);
					sprintf(buf, "attack damage: %.1f", level->all.tris[i].enemy->attack_damage);
					float_slider(&level->all.tris[i].enemy->attack_damage, buf, 0, 50);
					sprintf(buf, "attack range: %.1fm", level->all.tris[i].enemy->attack_range);
					float_slider(&level->all.tris[i].enemy->attack_range, buf, 0, 10);
					sprintf(buf, "projectile speed: %.1fm/s (0 = no projectile)", level->all.tris[i].enemy->projectile_speed);
					float_slider(&level->all.tris[i].enemy->projectile_speed, buf, 0, 50);
					sprintf(buf, "projectile scale: %.2f", level->all.tris[i].enemy->projectile_scale);
					float_slider(&level->all.tris[i].enemy->projectile_scale, buf, 0.1, 5);
				}
				// call("flip normal");
				// call("set animation start");
				// call("set animation stop");
				// slider("animation length", 0, 60);
				// button("\tforce disable culling");
				// button("\breakable");
				selected_index = i + 1;
			}
			else
			{
				if (level->all.tris[selected_index - 1].isenemy && !level->all.tris[i].enemy)
					init_enemy(&level->all.tris[i]);
				copy_tri_settings(&level->all.tris[i], &level->all.tris[selected_index - 1]);
			}
		}
	}
}

void	set_obj(t_level *level, char *filename)
{
	free_culling(level);
	free(level->all.tris);
	free(level->visible.tris);
	load_obj(filename, &level->all);
	if (!(level->visible.tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	init_screen_space_partition(level);
	init_culling(level);
}

void	set_texture(t_level *level, char *filename)
{
	free(level->texture.image);
	level->texture = bmp_read(filename);
}

void	set_skybox(t_level *level, char *filename)
{
	free(level->sky.img.image);
	level->sky.img = bmp_read(filename);
}

void	set_spawn_pos(t_level *level)
{
	level->spawn_pos.pos = level->cam.pos;
	level->spawn_pos.look_side = level->cam.look_side;
	level->spawn_pos.look_up = level->cam.look_up;
}

void	set_menu_pos_1(t_level *level)
{
	level->main_menu_pos1.pos = level->cam.pos;
	level->main_menu_pos1.look_side = level->cam.look_side;
	level->main_menu_pos1.look_up = level->cam.look_up;
}

void	set_menu_pos_2(t_level *level)
{
	level->main_menu_pos2.pos = level->cam.pos;
	level->main_menu_pos2.look_side = level->cam.look_side;
	level->main_menu_pos2.look_up = level->cam.look_up;
}

void	go_in_dir(char *path, char *folder)
{
	int i;
	int j;

	i = 0;
	j = 0;
	while (path[i])
		i++;
#ifdef __APPLE__
	path[i] = '/';
#elif _WIN32
	path[i] = '\\';
#endif
	i++;
	while (folder[j])
	{
		path[i] = folder[j];
		i++;
		j++;
	}
	path[i] = '\0';
}

void	make_fileopen_call(t_level *level, char *file)
{
	char absolute_filename[PATH_MAX + NAME_MAX];

	ft_strcpy(absolute_filename, level->ui.state.directory);
	go_in_dir(absolute_filename, file);
	level->ui.state.open_file(level, absolute_filename);
}

void	path_up_dir(char *path)
{
	int i;

	i = 0;
	while (path[i])
		i++;
#ifdef __APPLE__
	while (i && path[i] != '/')
#elif _WIN32
	while (i && path[i] != '\\')
#endif
		i--;
	if (i)
		path[i] = '\0';
}

#ifdef __APPLE__
void	ui_render_directory_loopdir(t_level *level, int find_dir, char *extension, int find)
{
	DIR *dir = opendir(level->ui.state.directory);

	if (!dir)
		ft_error("Cannot open directory\n");
	struct dirent *ent;
	int type = find_dir ? DT_DIR : DT_REG;
	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_type == type && ent->d_name[0] != '.')
			if (type == DT_DIR ||
			(!find && ft_strlen(ent->d_name) > ft_strlen(extension) &&
			ft_strcmp(extension, &ent->d_name[ft_strlen(ent->d_name) - ft_strlen(extension)])) ||
			(find && ft_strlen(ent->d_name) > ft_strlen(extension) &&
			!ft_strcmp(extension, &ent->d_name[ft_strlen(ent->d_name) - ft_strlen(extension)])))
			{
				if (type == DT_REG && call(ent->d_name, NULL, level))
					make_fileopen_call(level, ent->d_name);
				else if (type == DT_DIR && call(ent->d_name, NULL, level))
					go_in_dir(level->ui.state.directory, ent->d_name);
			}
	}
	closedir(dir);
}

#elif _WIN32
void	ui_render_directory_loopdir(t_level *level, int find_dir, char *extension, int find_ext)
{
	WIN32_FIND_DATA data;
	HANDLE			dir;
	char			*dirname;

	dirname = ft_strjoin(level->ui.state.directory, "\\*");
	dir = FindFirstFile(dirname, &data);
	if (dir == INVALID_HANDLE_VALUE)
		ft_error("Cannot open directory\n");
	while (FindNextFile(dir, &data))
	{
		if (data.cFileName[0] != '.')
			if (find_dir && data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && call(data.cFileName, NULL, level))
				go_in_dir(level->ui.state.directory, data.cFileName);
			else if (!find_dir)
				if (find_ext && ft_strlen(data.cFileName) > ft_strlen(extension) &&
				!ft_strcmp(extension, &data.cFileName[ft_strlen(data.cFileName) - ft_strlen(extension)]) &&
				call(data.cFileName, NULL, level))
				{
					if (level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
						make_fileopen_call(level, data.cFileName);
				}
				else if (!find_ext && ft_strlen(data.cFileName) > ft_strlen(extension) &&
				ft_strcmp(extension, &data.cFileName[ft_strlen(data.cFileName) - ft_strlen(extension)]) &&
				call(data.cFileName, NULL, level))
				{
					if (level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
						make_fileopen_call(level, data.cFileName);
				}
	}
	FindClose(dir);
	free(dirname);
}
#endif

void	ui_render_directory(t_level *level)
{
	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	text(level->ui.state.directory);
	if (call("close", NULL, level))
		level->ui.state.ui_location = UI_LOCATION_MAIN;
	if (call("up dir ..", NULL, level))
		path_up_dir(level->ui.state.directory);
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	ui_render_directory_loopdir(level, 1, NULL, 0);
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	ui_render_directory_loopdir(level, 0, level->ui.state.extension, 1);
	set_text_color(UI_INFO_TEXT_COLOR);
	ui_render_directory_loopdir(level, 0, level->ui.state.extension, 0);
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

int		nothing_selected(t_level *level)
{
	int i;

	i = 0;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected)
			return (0);
		i++;
	}
	return (1);
}

void	ui_config(t_level *level)
{
	char				 buf[100];
	t_editor_ui			*ui;

	ui = &level->ui;
	if (level->ui.state.ui_location == UI_LOCATION_FILE_SAVE || level->ui.state.ui_location == UI_LOCATION_FILE_OPEN)
	{
		ui_render_directory(level);
		return ;
	}
	if (level->ui.state.ui_location == UI_LOCATION_UV_EDITOR)
	{
		set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
		if (call("close uv editor", NULL, level))
			level->ui.state.ui_location = UI_LOCATION_MAIN;
		call("fix selected uv overlap", &fix_uv_overlap, level);
		return ;
	}
	if (level->ui.state.ui_location == UI_LOCATION_DOOR_EDITOR)
	{
		set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
		if (call("close door editor", NULL, level))
			level->ui.state.ui_location = UI_LOCATION_MAIN;
		find_selected_door_index(level);
		if (level->doors.selected_index)
		{
			call("delete selected door", &delete_door, level);
			call("set door start position", &set_door_pos_1, level);
			call("set door stop position", &set_door_pos_2, level);
			sprintf(buf, "door transition time: %fs", level->doors.door[level->doors.selected_index - 1].transition_time);
			float_slider(&level->doors.door[level->doors.selected_index - 1].transition_time, buf, .2, 7);
			// button(hinge, "has hinge");
			//if has hinge
			//	int_slider("hinge axis", 0, 2);
		}
		else
		{
			int selected = 0;
			for (int i = 0; i < level->all.tri_amount; i++)
				if (level->all.tris[i].selected)
					selected++;
			if (selected)
				call("new door from selection", &add_new_door, level);
			else
				text("Select faces to create door from");
		}
		return ;
	}
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	if (level->ui.state.ui_location == UI_LOCATION_SETTINGS)
	{
		sprintf(buf, "render scale: %d (%.0f%%)", ui->raycast_quality, 100.0 / (float)ui->raycast_quality);
		int_slider(&ui->raycast_quality, buf, 1, 20);
		sprintf(buf, "fov: %d", (int)((float)(ui->fov + 0.01) * (180.0 / M_PI)));
		float_slider(&ui->fov, buf, M_PI / 6, M_PI);
		button(&ui->blur, "blur");
		button(&ui->smooth_pixels, "smooth pixel transition");
		button(&ui->state.ssp_visual, "ssp visualize");
		return ;
	}
	button(&ui->noclip, "noclip");
	button(&ui->vertex_select_mode, "vertex select mode");
	button(&ui->wireframe, "wireframe");
	if (ui->wireframe)
	{
		button(&ui->wireframe_on_top, "wireframe on top");
		button(&ui->show_quads, "quad visualize");
		button(&ui->wireframe_culling_visual, "culling visualize");
	}

	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	call("edit uv", &enable_uv_editor, level);
	call("edit doors", &enable_door_editor, level);
	if (nothing_selected(level))
	{
		text("level:");
		file_browser("select level", ".doom-nukem", &open_level);
		file_browser("select obj", ".obj", &set_obj);
		file_browser("select texture", ".bmp", &set_texture);
		file_browser("select skybox", ".bmp", &set_skybox);
		call("add face", &add_face, level);
		call("set spawn position", &set_spawn_pos, level);
		call("set menu position 1", &set_menu_pos_1, level);
		call("set menu position 2", &set_menu_pos_2, level);
		sprintf(buf, "main menu animation time %ds", level->main_menu_anim_time);
		int_slider(&level->main_menu_anim_time, buf, 2, 50);
		button(&ui->fog, "fog");
		// color(ui->color, "fog color");
		// call(, "set spawn point");
		// call(, "spawn enemy");
		// call(, "remove enemies");
		// button(&ui->pause_culling_position, "\tpause");
		button(&ui->backface_culling, "backface & occlusion culling");
		button(&ui->distance_culling, "distance culling");
		sprintf(buf, "render distance: %.1fm", ui->render_distance);
		float_slider(&ui->render_distance, buf, 2, 50);
		float_slider(&ui->sun_contrast, "sun", 0, 1);
		float_slider(&ui->direct_shadow_contrast, "shadow", 0, 1);
		if (ui->sun_contrast > ui->direct_shadow_contrast)
			ui->direct_shadow_contrast = ui->sun_contrast;
		sprintf(buf, "sun dir: (%.2f, %.2f, %.2f)", ui->sun_dir.x, ui->sun_dir.y, ui->sun_dir.z);
		text(buf);
		//vec3_slider
		float_slider(&ui->sun_dir.x, NULL, -1, 1);
		float_slider(&ui->sun_dir.y, NULL, -1, 1);
		float_slider(&ui->sun_dir.z, NULL, -1, 1);
		vec_normalize(&ui->sun_dir);
		file_save("save level", ".doom-nukem", NULL);

		set_text_color(UI_INFO_TEXT_COLOR);
		sprintf(buf, "fps:               %d",  get_fps());
		text(buf);
		sprintf(buf, "cull:              %ums",  ui->cull);
		text(buf);
		sprintf(buf, "ssp:               %ums",  ui->ssp);
		text(buf);
		sprintf(buf, "render:          %ums",  ui->render);
		text(buf);
		sprintf(buf, "frametime: %ums",  ui->frametime);
		text(buf);
		sprintf(buf, "faces:           %d / %d", level->all.tri_amount, level->visible.tri_amount);
		text(buf);
		sprintf(buf, "xz velocity:  %.2fms", level->ui.horizontal_velocity);
		text(buf);
	}

	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	ui_config_selected_faces(level);
}
