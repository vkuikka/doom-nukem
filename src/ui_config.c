/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/27 01:03:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/02 07:44:11 by rpehkone         ###   ########.fr       */
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

void	ui_config_selected_faces(t_level *level)
{
	char buf[100];
	// int	selected_amount;

	// selected_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		int counter = 0;
		int j = 0;
		while (j < 3 + level->all.tris[i].isquad)
		{
			if (level->all.tris[i].verts[j].selected)
				counter++;
			j++;
		}
			// selected_amount++;
		if (counter == 4)
		{
			text("");
			//if (selected_amount == 1)
				text("Selected face:");
			// else if (selected_amount > 1)
				// text("%d selected faces (toggle all):");
			sprintf(buf, "reflectivity: %.0f%%", 100 * level->all.tris[i].reflectivity);
			float_slider(&level->all.tris[i].reflectivity, buf, 0, 1);
			sprintf(buf, "opacity: %.0f%%", 100 * level->all.tris[i].opacity);
			float_slider(&level->all.tris[i].opacity, buf, 0, 1);
			button(&level->all.tris[i].isgrid, "grid");
			button(&level->all.tris[i].isenemy, "enemy");
			// call("flip normal");
			// call("set animation start");
			// call("set animation stop");
			// slider("animation length", 0, 60);
			// button("\tforce disable culling");
			// button("\breakable");
		}
	}
}

void	set_ui_dir(t_level *level)
{
}

void	set_obj(t_level *level, char *filename)
{
	//free obj
	load_obj(filename, &level->all);
}

void	set_skybox(t_level *level, char *filename)
{
	//free skybox
	level->sky.img = bmp_read(filename);
}


void	ui_render_directory_loopdir(t_level *level, int type, char *extension, int find)
{
	DIR *dir = opendir(level->ui->state.directory);

	if (!dir)
		ft_error("Cannot open directory\n");
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_type == type && ent->d_name[0] != '.')
			if (type == DT_DIR ||
			(!find && ft_strlen(ent->d_name) > ft_strlen(extension) &&
			ft_strcmp(extension, &ent->d_name[ft_strlen(ent->d_name) - ft_strlen(extension)])) ||
			(find && ft_strlen(ent->d_name) > ft_strlen(extension) &&
			!ft_strcmp(extension, &ent->d_name[ft_strlen(ent->d_name) - ft_strlen(extension)])))
			{
				if (type == DT_REG && call(ent->d_name, &set_ui_dir, level))
					level->ui->state.open_file(level, ent->d_name);
				else if (type == DT_DIR && call(ent->d_name, &set_ui_dir, level))
					ft_strcpy(level->ui->state.directory, ent->d_name)
					;//directory = (level, ent->d_name);
			}
	}
	closedir(dir);
}

void	ui_render_directory(t_level *level)
{
	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	if (call("close", &set_ui_dir, level))
		level->ui->state.is_directory_open = 0;
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	if (call("..", NULL, level))
		ft_strcpy(level->ui->state.directory, "..");
	ui_render_directory_loopdir(level, DT_DIR, NULL, 0);
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	ui_render_directory_loopdir(level, DT_REG, level->ui->state.extension, 1);
	set_text_color(UI_INFO_TEXT_COLOR);
	ui_render_directory_loopdir(level, DT_REG, level->ui->state.extension, 0);
}


void	ui_config(t_level *level)
{
	char				 buf[100];
	t_editor_ui			*ui;

	ui = level->ui;
	if (level->ui->state.is_directory_open)
	{
		ui_render_directory(level);
		return ;
	}
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	button(&ui->noclip, "noclip");
	button(&ui->wireframe, "wireframe");
	button(&ui->wireframe_on_top, "wireframe on top");
	button(&ui->show_quads, "quad visualize");
	button(&ui->wireframe_culling_visual, "culling visualize");
	// button(, "face/vert selection");
	sprintf(buf, "render scale: %d (%.0f%%)", ui->raycast_quality,100.0 / (float)ui->raycast_quality);
	int_slider(&ui->raycast_quality, buf, 1, 20);

	text("");
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	text("level:");
	//file_browser("select level", ".dnukem_level", &set_level;
	file_browser("select obj", ".obj", &set_obj);
	// file_browser("select texture", ".bmp", &set_texture;
	file_browser("select skybox", ".bmp", &set_skybox);
	button(&ui->fog, "fog");
	// color(ui->color, "fog color");
	// call(, "set spawn point");
	// call(, "spawn enemy");
	// call(, "remove enemies");
	text("culling:");
	// button(&ui->pause_culling_position, "\tpause");
	button(&ui->backface_culling, "backface");
	button(&ui->distance_culling, "distance");
	sprintf(buf, "render distance: %.1fm", ui->render_distance);
	float_slider(&ui->render_distance, buf, 2, 50);
	float_slider(&ui->sun_contrast, "sun", 0, 1);
	float_slider(&ui->direct_shadow_contrast, "shadow", 0, 1);
	sprintf(buf, "sun dir: (%.2f, %.2f, %.2f)", ui->sun_dir.x, ui->sun_dir.y, ui->sun_dir.z);
	text(buf);
	//vec3_slider
	float_slider(&ui->sun_dir.x, NULL, -1, 1);
	float_slider(&ui->sun_dir.y, NULL, -1, 1);
	float_slider(&ui->sun_dir.z, NULL, -1, 1);
	vec_normalize(&level->ui->sun_dir);

	text("");
	set_text_color(UI_INFO_TEXT_COLOR);
	sprintf(buf, "fps:               %d",  get_fps());
	text(buf);
	sprintf(buf, "tickrate:      %.1fhz",  ui->physhz);
	text(buf);
	sprintf(buf, "frametime: %ums",  ui->frametime);
	text(buf);
	sprintf(buf, "faces:           %d / %d", level->all.tri_amount, level->visible.tri_amount);
	text(buf);
	sprintf(buf, "faces left:    %d", level->ssp[0].tri_amount);
	text(buf);
	sprintf(buf, "faces right:  %d",  level->ssp[1].tri_amount);
	text(buf);

	set_text_color(UI_FACE_SELECTION_TEXT_COLOR);
	ui_config_selected_faces(level);
}
