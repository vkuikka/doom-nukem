/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uv_editor.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/08 10:37:45 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/08 10:37:45 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void		uv_pixel_put(int x, int y, int color, unsigned *texture)
{
	int oppacity;

	if (x < 0 || y < 0 || x > RES_X / 2 || y >= RES_Y)
		return;
	oppacity = (color << (8 * 3)) >> (8 * 3);
	if (!oppacity)
		texture[x + (y * RES_X)] = UI_BACKGROUND_COL;
	else
		texture[x + (y * RES_X)] = color;
}

static float	get_texture_scale(t_bmp *img)
{
	t_vec2	res;

	res.x = (float)RES_X / 2 / img->width;
	res.y = (float)RES_Y / img->height;
	if (res.y < res.x)
		return (res.y);
	else
		return (res.x);
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

static void		find_closest_to_mouse(t_vec2 *vert, int *i, int *k)
{
	static float	nearest_len = -1;
	static int		nearest_tri_index = -1;
	static int		nearest_vert_index = -1;
	float			len;
	t_vec2			test;
	int				x;
	int				y;

	if (!vert)
	{
		*k = nearest_vert_index;
		*i = nearest_tri_index;
		nearest_vert_index = -1;
		nearest_tri_index = -1;
		nearest_len = -1;
		return ;
	}
	SDL_GetMouseState(&x, &y);
	test = *vert;
	test.x -= x;
	test.y -= y;
	len = vec2_length(test);
	if (len < nearest_len || nearest_len == -1)
	{
		nearest_len = len;
		nearest_tri_index = *i;
		nearest_vert_index = *k;
	}
}

static void		update_uv_closest_vertex(t_level *level, float image_scale)
{
	int	i;
	int	k;
	int	x;
	int	y;

	find_closest_to_mouse(NULL, &i, &k);
	if (i != -1 && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT) && x < RES_X / 2)
	{
		y -= 18 - RES_Y / 2 + (level->texture.height * image_scale) / 2;
		y -= level->texture.height * image_scale / 2;
		level->all.tris[i].verts[k].txtr.x = x / (level->texture.width * image_scale);
		level->all.tris[i].verts[k].txtr.y = 1 - y / (level->texture.height * image_scale);
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

static void		uv_wireframe(t_level *level, unsigned *pixels, float image_scale)
{
	int		y_offset;
	int		next;
	t_ivec2	color;
	t_vec2	start;
	t_vec2	stop;

	y_offset = 18 - RES_Y / 2 + (level->texture.height * image_scale) / 2;
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
				start.y = (level->texture.height * image_scale) * -level->all.tris[i].verts[k].txtr.y + RES_Y;
				stop.x = (level->texture.width * image_scale) * level->all.tris[i].verts[next].txtr.x;
				stop.y = (level->texture.height * image_scale) * -level->all.tris[i].verts[next].txtr.y + RES_Y;
				start.y += y_offset;
				stop.y += y_offset;
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
					find_closest_to_mouse(&start, &i, &k);
				}
			}
		}
	}
	update_uv_closest_vertex(level, image_scale);
}

void			uv_editor(t_level *level, t_window *window)
{
	static SDL_Texture	*texture = NULL;
	static unsigned		*pixels;
	signed				width;
	float				image_scale;
	int					y_offset;

	if (!texture)
	{
		texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width))
			ft_error("failed to lock texture\n");
	}
	image_scale = get_texture_scale(&level->texture);
	y_offset = 18 + RES_Y / 2 - ((level->texture.height * image_scale) / 2);
	for (int y = 0; y < RES_Y; y++)
		for (int x = 0; x < RES_X / 2; x++)
			uv_pixel_put(x, y, UI_BACKGROUND_COL, pixels);
	for (int y = 0; y < RES_Y; y++)
		for (int x = 0; x < RES_X / 2; x++)
			if (x / image_scale < level->texture.width && y / image_scale < level->texture.height)
				uv_pixel_put(x, y + y_offset, level->texture.image[(int)(y / image_scale) * level->texture.width + (int)(x / image_scale)], pixels);
	uv_wireframe(level, pixels, image_scale);
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
