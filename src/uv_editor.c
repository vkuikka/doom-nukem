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

#include "doom-nukem.h"

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
		uv_pixel_put(pos.x, pos.y, crossfade(color.x, color.y, 0xff * (pos.z / step.z)), pixels);
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

static void	set_fourth_vertex_uv(t_tri *a)
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

static void		draw_projectile_uv(t_level *level, t_ivec2 offset, unsigned *pixels, float image_scale, int i)
{
	t_ivec2	mouse;
	t_ivec2	color;
	t_vec2	start;
	t_vec2	stop;
	int		next;

	for (int k = 0; k < 3; k++)
	{
		next = (k + 1) % 3;
		start.x = (level->texture.width * image_scale) * level->all.tris[i].enemy->projectile_uv[k].x;
		start.y = (level->texture.height * image_scale) * (1 - level->all.tris[i].enemy->projectile_uv[k].y);
		stop.x = (level->texture.width * image_scale) * level->all.tris[i].enemy->projectile_uv[next].x;
		stop.y = (level->texture.height * image_scale) * (1 - level->all.tris[i].enemy->projectile_uv[next].y);
		start.y += offset.y;
		start.x += offset.x;
		stop.y += offset.y;
		stop.x += offset.x;
		color.x = WF_PROJECTILE_COL >> 8;
		color.y = WF_PROJECTILE_COL >> 8;
		uv_print_line(start, stop, color, pixels);
		put_uv_vertex(start, WF_PROJECTILE_COL, pixels);
		SDL_GetMouseState(&mouse.x, &mouse.y);
		find_closest_to_mouse(level, &level->all.tris[i].enemy->projectile_uv[k], &start, &mouse);
	}
}

static void		uv_wireframe(t_level *level, t_ivec2 offset, unsigned *pixels, float image_scale)
{
	int		next;
	t_ivec2	mouse;
	t_ivec2	color;
	t_vec2	start;
	t_vec2	stop;

	SDL_GetMouseState(&mouse.x, &mouse.y);
	next = -1;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].selected)
		{
			for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
			{
				if (level->all.tris[i].isquad)
					next = (int[4]){1, 3, 0, 2}[k];
				else
					next = (k + 1) % 3;
				start.x = (level->texture.width * image_scale) * level->all.tris[i].verts[k].txtr.x;
				start.y = (level->texture.height * image_scale) * (1 - level->all.tris[i].verts[k].txtr.y);
				stop.x = (level->texture.width * image_scale) * level->all.tris[i].verts[next].txtr.x;
				stop.y = (level->texture.height * image_scale) * (1 - level->all.tris[i].verts[next].txtr.y);
				start.y += offset.y;
				start.x += offset.x;
				stop.y += offset.y;
				stop.x += offset.x;
				color.x = WF_SELECTED_COL >> 8;
				color.y = WF_SELECTED_COL >> 8;
				if (k == 3)
					color.x = WF_UNSELECTED_COL >> 8;
				if (next == 3)
					color.y = WF_UNSELECTED_COL >> 8;
				uv_print_line(start, stop, color, pixels);
				if (k == 3)
					put_uv_vertex(start, 0xff, pixels);
				else
				{
					set_fourth_vertex_uv(&level->all.tris[i]);
					put_uv_vertex(start, WF_SELECTED_COL, pixels);
					find_closest_to_mouse(level, &level->all.tris[i].verts[k].txtr, &start, &mouse);
				}
			}
			if (level->all.tris[i].enemy)
			{
				draw_projectile_uv(level, offset, pixels, image_scale, i);
				for (int k = 0; k < 3; k++)
					find_closest_to_mouse(level, &level->all.tris[i].enemy->projectile_uv[k], &start, &mouse);
			}
		}
	}
	if (next != -1)
		update_uv_closest_vertex(level, image_scale, offset, mouse);
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
			if (x - offset.x >= 0 && x - offset.x < level->texture.width * image_scale &&
				y - offset.y >= 0 && y - offset.y < level->texture.height * image_scale)
				uv_pixel_put(x, y, level->texture.image[(int)((y - offset.y) / image_scale) * level->texture.width +
														(int)((x - offset.x) / image_scale)], pixels);
	uv_wireframe(level, offset, pixels, image_scale);
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
	if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width))
		ft_error("failed to lock texture\n");
	ft_memset(pixels, 0, RES_X * RES_Y * 4);
}

void	disable_uv_editor(t_level *level)
{
	level->ui.state.is_uv_editor_open = FALSE;
}

void	enable_uv_editor(t_level *level)
{
	level->ui.state.is_uv_editor_open = TRUE;
	level->ui.wireframe = TRUE;
	level->ui.wireframe_on_top = TRUE;
}
