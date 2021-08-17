/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_editor.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 10:37:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/01 17:26:135y vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

static void		uv_pixel_put(int x, int y, int color, unsigned *texture)
{
	int opacity;

	if (x < 0 || y < 0 || x >= RES_X / 2 || y >= RES_Y)
		return;
	opacity = (color << (8 * 3)) >> (8 * 3);
	if (!opacity)
		texture[x + (y * RES_X)] = UI_BACKGROUND_COL;
	else
		texture[x + (y * RES_X)] = color;
}

static float	get_texture_scale(t_bmp *img)
{
	if (img->width < img->height)
		return ((float)(RES_Y - UV_EDITOR_Y_OFFSET) / img->height);
	else
		return ((float)RES_X / 2 / img->width);
}

void	uv_print_line(t_vec2 start, t_vec2 stop, t_ivec2 color, unsigned *pixels)
{
	t_vec3	step;
	t_vec3	pos;
	int		i;

	i = 0;
	pos.x = start.x;
	pos.y = start.y;
	pos.z = 0;
	step.z = ft_abs(stop.x - start.x) > ft_abs(stop.y - start.y) ?
			ft_abs(stop.x - start.x) : ft_abs(stop.y - start.y);
	step.x = (stop.x - start.x) / (float)step.z;
	step.y = (stop.y - start.y) / (float)step.z;
	while (pos.z <= step.z && i < 10000)
	{
		uv_pixel_put(pos.x, pos.y, crossfade(color.x, color.y, 0xff * (pos.z / step.z), 0xff), pixels);
		pos.x += step.x;
		pos.y += step.y;
		pos.z++;
		i++;
	}
}

static void		put_uv_vertex(t_vec2 vertex, int color, unsigned *pixels)
{
	int	a;
	int	b;

	a = -1;
	while (a < 2)
	{
		b = -1;
		while (b < 2)
		{
			uv_pixel_put(vertex.x + a, vertex.y + b, color, pixels);
			b++;
		}
		a++;
	}
}

static t_vec2	*find_closest_to_mouse(t_level *level, t_vec2 *txtr, t_vec2 *screen_pos, t_ivec2 *mouse)
{
	static float	closest_dist = -1;
	static t_vec2	*closest = NULL;
	t_vec2			*res;
	t_vec2			tmp;
	float			dist;

	if (!txtr || !screen_pos || !mouse)
	{
		res = closest;
		if (!level->ui.state.m1_drag)
		{
			closest = NULL;
			closest_dist = -1;
		}
		return (res);
	}
	tmp.x = screen_pos->x - mouse->x;
	tmp.y = screen_pos->y - mouse->y;
	dist = vec2_length(tmp);
	if (level->ui.state.m1_click && (dist < closest_dist || closest_dist == -1))
	{
		closest_dist = dist;
		closest = txtr;
	}
	return (NULL);
}

static void		update_uv_closest_vertex(t_level *level, float image_scale, t_ivec2 offset, t_ivec2 mouse)
{
	t_vec2	*closest;

	closest = find_closest_to_mouse(level, NULL, NULL, NULL);
	if (closest && level->ui.state.mouse_location == MOUSE_LOCATION_UV_EDITOR && level->ui.state.m1_drag)
	{
		mouse.x -= offset.x;
		mouse.y -= offset.y;
		closest->x = mouse.x / (level->texture.width * image_scale);
		closest->y = 1 - (mouse.y / (level->texture.height * image_scale));
	}
}

void			set_fourth_vertex_uv(t_tri *a)
{
	t_vec2 shared1;
	t_vec2 shared2;
	t_vec2 avg;
	t_vec2 new;
	t_vec2 res;

	vec2_sub(&shared1, a->verts[1].txtr, a->verts[0].txtr);
	vec2_sub(&shared2, a->verts[2].txtr, a->verts[0].txtr);
	vec2_avg(&avg, shared1, shared2);
	vec2_add(&new, avg, avg);
	vec2_add(&res, new, a->verts[0].txtr);
	a->verts[3].txtr.x = res.x;
	a->verts[3].txtr.y = res.y;
}

static void		draw_uv(t_level *level, t_uv_parameters param, t_vec2 uv[3], t_ivec2 mouse)
{
	t_ivec2	color;
	t_vec2	start;
	t_vec2	stop;
	int		next;
	int		k;

	k = 0;
	while (k < 3)
	{
		next = (k + 1) % 3;
		start.x = param.scale.x * uv[k].x;
		start.y = param.scale.y * (1 - uv[k].y);
		stop.x = param.scale.x * uv[next].x;
		stop.y = param.scale.y * (1 - uv[next].y);
		vec2_add(&start, start, param.offset);
		vec2_add(&stop, stop, param.offset);
		color.x = WF_PROJECTILE_COL >> 8;
		color.y = WF_PROJECTILE_COL >> 8;
		uv_print_line(start, stop, color, param.pixels);
		put_uv_vertex(start, WF_PROJECTILE_COL, param.pixels);
		find_closest_to_mouse(level, &uv[k], &start, &mouse);
		k++;
	}
}

static void		draw_face_uv(t_level *level, t_uv_parameters param, t_ivec2 mouse)
{
	t_ivec2		color;
	t_vec2		start;
	t_vec2		stop;
	int			k;
	int			next;

	k = 0;
	while (k < 3 + param.tri->isquad)
	{
		if (param.tri->isquad)
			next = (int[4]){1, 3, 0, 2}[k];
		else
			next = (k + 1) % 3;
		start.x = param.scale.x * param.tri->verts[k].txtr.x;
		start.y = param.scale.y * (1 - param.tri->verts[k].txtr.y);
		stop.x = param.scale.x * param.tri->verts[next].txtr.x;
		stop.y = param.scale.y * (1 - param.tri->verts[next].txtr.y);
		vec2_add(&start, start, param.offset);
		vec2_add(&stop, stop, param.offset);
		color.x = WF_SELECTED_COL >> 8;
		color.y = WF_SELECTED_COL >> 8;
		if (k == 3)
			color.x = WF_UNSELECTED_COL >> 8;
		if (next == 3)
			color.y = WF_UNSELECTED_COL >> 8;
		uv_print_line(start, stop, color, param.pixels);
		if (k == 3)
			put_uv_vertex(start, 0xff, param.pixels);
		else
		{
			set_fourth_vertex_uv(param.tri);
			put_uv_vertex(start, WF_SELECTED_COL, param.pixels);
			find_closest_to_mouse(level, &param.tri->verts[k].txtr, &start, &mouse);
		}
		k++;
	}
}

static void		uv_wireframe(t_level *level, t_ivec2 offset, unsigned *pixels, float image_scale)
{
	t_ivec2	mouse;
	t_vec2	start;
	t_vec2	stop;
	t_uv_parameters		param;

	SDL_GetMouseState(&mouse.x, &mouse.y);
	param.offset.x = offset.x;
	param.offset.y = offset.y;
	param.scale.x = level->texture.width * image_scale;
	param.scale.y = level->texture.height * image_scale;
	param.pixels = pixels;
	if (nothing_selected(level))
	{
		draw_uv(level, param, level->player.projectile_uv, mouse);
		for (int k = 0; k < 3; k++)
			find_closest_to_mouse(level, &level->player.projectile_uv[k], &start, &mouse);
	}
	else
		for (int i = 0; i < level->all.tri_amount; i++)
			if (level->all.tris[i].selected)
			{
				param.tri = &level->all.tris[i];
				draw_face_uv(level, param, mouse);
				if (level->all.tris[i].enemy)
				{
					draw_uv(level, param, level->all.tris[i].enemy->projectile_uv, mouse);
					for (int k = 0; k < 3; k++)
						find_closest_to_mouse(level, &level->all.tris[i].enemy->projectile_uv[k], &start, &mouse);
				}
			}
	update_uv_closest_vertex(level, image_scale, offset, mouse);
}

static void		match_projectile_uv(t_level *level)
{
	int 	i;
	int		copy_index;

	i = 0;
	copy_index = -1;
	while (i < level->all.tri_amount)
	{
		if (level->all.tris[i].selected && level->all.tris[i].isenemy)
		{
			if (copy_index != -1)
			{
				level->all.tris[i].enemy->projectile_uv[0] = level->all.tris[copy_index].enemy->projectile_uv[0];
				level->all.tris[i].enemy->projectile_uv[1] = level->all.tris[copy_index].enemy->projectile_uv[1];
				level->all.tris[i].enemy->projectile_uv[2] = level->all.tris[copy_index].enemy->projectile_uv[2];
			}
			else
				copy_index = i;
		}
		i++;
	}
}

void			uv_editor(t_level *level, t_window *window)
{
	static SDL_Texture	*texture = NULL;
	static unsigned		*pixels;
	signed				width;
	float				image_scale;
	t_ivec2				offset;

	if (!texture)
	{
		texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width))
			ft_error("failed to lock texture\n");
	}
	image_scale = get_texture_scale(&level->texture) * level->ui.state.uv_zoom;
	offset.x = level->ui.state.uv_pos.x;
	offset.y = UV_EDITOR_Y_OFFSET + level->ui.state.uv_pos.y;
	offset.x += RES_X / 4;
	offset.y += RES_Y / 2;
	offset.x -= level->texture.width * image_scale / 2;
	offset.y -= level->texture.height * image_scale / 2;
	for (int y = 0; y < RES_Y; y++)
		for (int x = 0; x < RES_X / 2; x++)
			uv_pixel_put(x, y, UI_BACKGROUND_COL, pixels);
	for (int y = 0 ; y < RES_Y; y++)
		for (int x = 0 ; x < RES_X / 2; x++)
			if (x - offset.x >= 0 && x - offset.x < (int)(level->texture.width * image_scale) &&
				y - offset.y >= 0 && y - offset.y < (int)(level->texture.height * image_scale))
				uv_pixel_put(x, y, level->texture.image[(int)((y - offset.y) / image_scale) * level->texture.width +
														(int)((x - offset.x) / image_scale)], pixels);
	match_projectile_uv(level);
	uv_wireframe(level, offset, pixels, image_scale);
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
	if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width))
		ft_error("failed to lock texture\n");
	ft_memset(pixels, 0, RES_X * RES_Y * 4);
}

void	enable_uv_editor(t_level *level)
{
	level->ui.state.ui_location = UI_LOCATION_UV_EDITOR;
	level->ui.wireframe = TRUE;
	level->ui.wireframe_on_top = TRUE;
}
