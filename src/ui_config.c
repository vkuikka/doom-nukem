/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/27 01:03:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/03 23:29:12 by vkuikka          ###   ########.fr       */
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
	a->disable_distance_culling = b->disable_distance_culling;
	a->disable_backface_culling = b->disable_backface_culling;
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
					sprintf(buf, "%d faces selected (toggle all):", selected_amount);
					text(buf);
				}
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
				button(&level->all.tris[i].isgrid, "grid");
				button(&level->all.tris[i].isenemy, "enemy");
				button(&level->all.tris[i].shader, "water");
				button(&level->all.tris[i].disable_distance_culling, "distance culling");
				button(&level->all.tris[i].disable_backface_culling, "backface culling");
				// call("flip normal");
				// call("set animation start");
				// call("set animation stop");
				// slider("animation length", 0, 60);
				// button("\tforce disable culling");
				// button("\breakable");
				selected_index = i + 1;
			}
			else
				copy_tri_settings(&level->all.tris[i], &level->all.tris[selected_index - 1]);
		}
	}
}

void	set_obj(t_level *level, char *filename)
{
	free_reflection_culling(level);
	free(level->all.tris);
	free(level->visible.tris);
	load_obj(filename, &level->all);
	if (!(level->visible.tris = (t_tri*)malloc(sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed\n");
	init_screen_space_partition(level);
	init_reflection_culling(level);
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
					if (level->ui.state.is_directory_open)
						make_fileopen_call(level, data.cFileName);
				}
				else if (!find_ext && ft_strlen(data.cFileName) > ft_strlen(extension) &&
				ft_strcmp(extension, &data.cFileName[ft_strlen(data.cFileName) - ft_strlen(extension)]) &&
				call(data.cFileName, NULL, level))
				{
					if (level->ui.state.is_directory_open)
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
	{
		level->ui.state.is_serialize_open = FALSE;
		level->ui.state.is_directory_open = FALSE;
	}
	if (call("up dir ..", NULL, level))
		path_up_dir(level->ui.state.directory);
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	ui_render_directory_loopdir(level, 1, NULL, 0);
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	ui_render_directory_loopdir(level, 0, level->ui.state.extension, 1);
	set_text_color(UI_INFO_TEXT_COLOR);
	ui_render_directory_loopdir(level, 0, level->ui.state.extension, 0);
	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	if (level->ui.state.is_serialize_open)
	{
		text_input(level->ui.state.save_filename, level);
		if (call("save", NULL, level))
		{
			save_level(level);
			level->ui.state.is_serialize_open = FALSE;
			level->ui.state.is_directory_open = FALSE;
		}
	}
}

void	ui_config(t_level *level)
{
	char				 buf[100];
	t_editor_ui			*ui;

	ui = &level->ui;
	if (level->ui.state.is_serialize_open || level->ui.state.is_directory_open)
	{
		ui_render_directory(level);
		return ;
	}
	if (level->ui.state.is_uv_editor_open)
	{
		set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
		call("close uv editor", &disable_uv_editor, level);
		return ;
	}
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	// button(, "face/vert selection");
	sprintf(buf, "render scale: %d (%.0f%%)", ui->raycast_quality, 100.0 / (float)ui->raycast_quality);
	int_slider(&ui->raycast_quality, buf, 1, 20);
	sprintf(buf, "fov: %d", (int)((float)(ui->fov + 0.01) * (180.0 / M_PI)));
	float_slider(&ui->fov, buf, M_PI / 6, M_PI);
	button(&ui->noclip, "noclip");
	button(&ui->blur, "blur");
	button(&ui->smooth_pixels, "smooth pixel transition");
	button(&ui->wireframe, "wireframe");
	if (ui->wireframe)
	{
		button(&ui->wireframe_on_top, "wireframe on top");
		button(&ui->show_quads, "quad visualize");
		button(&ui->wireframe_culling_visual, "culling visualize");
	}
	button(&ui->state.ssp_visual, "ssp visualize");
	// button(, "face/vert selection");

	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	text("level:");
	file_browser("select level", ".doom-nukem", &open_level);
	file_browser("select obj", ".obj", &set_obj);
	file_browser("select texture", ".bmp", &set_texture);
	call("edit uv", &enable_uv_editor, level);
	file_browser("select skybox", ".bmp", &set_skybox);
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
	sprintf(buf, "frametime: %ums",  ui->frametime);
	text(buf);
	sprintf(buf, "faces:           %d / %d", level->all.tri_amount, level->visible.tri_amount);
	text(buf);
	sprintf(buf, "xz velocity:  %.2fms", level->ui.horizontal_velocity);
	text(buf);

	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	ui_config_selected_faces(level);
}
