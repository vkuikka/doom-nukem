/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_config.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/27 01:03:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/03/30 10:03:29 by rpehkone         ###   ########.fr       */
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


void	ui_config(t_level *level)
{
	char				 buf[100];
	t_editor_ui			*ui;

	ui = level->ui;
	set_text_color(UI_EDITOR_SETTINGS_TEXT_COLOR);
	button(&ui->noclip, "noclip");
	button(&ui->wireframe, "wireframe");
	button(&ui->show_quads, "quad visualize");
	button(&ui->wireframe_culling_visual, "visualize culling in wireframe");
	// button(, "face/vert selection");
	sprintf(buf, "pixel gap %d", ui->raycast_quality);
	text(buf);
	sprintf(buf, "render scale %.0f%%", 100.0 / (float)ui->raycast_quality);
	int_slider(&ui->raycast_quality, buf, 1, 20);

	text("");
	set_text_color(UI_LEVEL_SETTINGS_TEXT_COLOR);
	text("level:");
	//file_browse("level", ".dnukem_level", &set_level(char *filename));
	//file_browse("map", ".obj", &set_map(char *filename));
	//file_browse("texture", ".bmp", &set_texture(char *filename));
	//file_browse("skybox", ".bmp", &set_skybox(char *filename));
	button(&ui->fog, "fog");
	// color(ui->color, "fog color");
	// call(, "set spawn point");
	// call(, "spawn enemy");
	// call(, "remove enemies");
	text("culling:");
	// button(&ui->pause_culling_position, "\tpause");
	button(&ui->backface_culling, "backface");
	button(&ui->distance_culling, "distance");
	int test = ui->render_distance;
	sprintf(buf, "render distance: %.1fm", ui->render_distance);
	int_slider(&test, buf, 2, 50);
	// slider(&ui->culling_distance, "culling distance", 2, 50);
	ui->render_distance = test;

	// text("Selected face:");
	// slider("oppacity", 0, 1);
	// slider("reflectivity", 0, 1);
	// call("flip normal");
	// slider("animation length", 0, 60);
	// call("set animation start");
	// call("set animation stop");
	// button("\tforce disable culling");
	// button("\tinfinite");
	// button("\breakable");
	//oisko jos hover n'it' hightlightaa facet miss' se on p''ll'

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
}
