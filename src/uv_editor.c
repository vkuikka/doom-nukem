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


void	uv_print_line(t_vec2 start, t_vec2 stop, int color, unsigned *pixels)
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
		uv_pixel_put(pos.x, pos.y, color, pixels);
		pos.x += step.x;
		pos.y += step.y;
		pos.z++;
		i++;
	}
}

static void		uv_wireframe(t_level *level, unsigned *pixels, float image_scale)
{
	int		y_offset;
	int		next;
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
				//scale and invert y
				start.x = (level->texture.width * image_scale) * level->all.tris[i].verts[k].txtr.x;
				start.y = (level->texture.height * image_scale) * -level->all.tris[i].verts[k].txtr.y + RES_Y;
				stop.x = (level->texture.width * image_scale) * level->all.tris[i].verts[next].txtr.x;
				stop.y = (level->texture.height * image_scale) * -level->all.tris[i].verts[next].txtr.y + RES_Y;
				start.y += y_offset;
				stop.y += y_offset;
				uv_print_line(start, stop, WF_SELECTED_COL, pixels);
			}
		}
	}
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
	//for selected faces
		//wireframe tri->uv
}

void	disable_uv_editor(t_level *level)
{
	level->ui.state.is_uv_editor_open = FALSE;
	level->ui.wireframe = FALSE;
}

void	enable_uv_editor(t_level *level)
{
	level->ui.state.is_uv_editor_open = TRUE;
	level->ui.wireframe = TRUE;
	level->ui.wireframe_on_top = TRUE;
	level->ui.wireframe_culling_visual = TRUE;
}