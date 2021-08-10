/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gizmo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/26 19:56:07 by rpehkone          #+#    #+#             */
/*   Updated: 2021/05/30 22:22:56 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void		gizmo_pixel_put(int x, int y, int color, unsigned *texture)
{
	if (x >= 0 && y >= 0 && x < RES_X && y < RES_Y)
        texture[x + (y * RES_X)] = color;
}

void	gizmo_print_line(t_vec3 start, t_vec3 stop, int color, unsigned *pixels)
{
	t_vec3	step;
	t_vec3	pos;
	int		i;

	i = 0;
	if (start.z < 0 || stop.z < 0)
		return ;
	pos.x = start.x;
	pos.y = start.y;
	pos.z = 0;
	step.z = ft_abs(stop.x - start.x) > ft_abs(stop.y - start.y) ?
			ft_abs(stop.x - start.x) : ft_abs(stop.y - start.y);
	step.x = (stop.x - start.x) / (float)step.z;
	step.y = (stop.y - start.y) / (float)step.z;
	while (pos.z <= step.z && i < 10000)
	{
		gizmo_pixel_put(pos.x, pos.y, color, pixels);
		pos.x += step.x;
		pos.y += step.y;
		pos.z++;
		i++;
	}
}

static float	scale_translation_gizmo(t_vec3 *x, t_vec3 *y, t_vec3 *z, t_vec3 avg)
{
	float dist;

	dist = vec_length(avg) / GIZMO_SCALE_DIVIDER;
	vec_mult(x, dist);
	vec_mult(y, dist);
	vec_mult(z, dist);
	return (dist);
}

t_vec3		vec_sub_return(t_vec3 ve1, t_vec3 ve2)
{
	t_vec3	res;

	res.x = ve1.x - ve2.x;
	res.y = ve1.y - ve2.y;
	res.z = ve1.z - ve2.z;
	return (res);
}

static void		put_gizmo_box(t_vec3 vertex, int color, unsigned *pixels)
{
	int	a;
	int	b;

	if (vertex.z < 0)
		return ;
	a = -6;
	while (a < 11)
	{
		b = -6;
		while (b < 11)
		{
			gizmo_pixel_put(vertex.x + a, vertex.y + b, color, pixels);
			b++;
		}
		a++;
	}
}

static void		render_translation_gizmo(t_level *level, unsigned *pixels)
{
	t_vec3	avg;
	t_vec3	x = {-1, 0, 0};
	t_vec3	y = {0, -1, 0};
	t_vec3	z = {0, 0, -1};

	avg = level->ui.state.gizmo_pos;
	scale_translation_gizmo(&x, &y, &z, vec_sub_return(avg, level->cam.pos));
	vec_add(&x, x, avg);
	vec_add(&y, y, avg);
	vec_add(&z, z, avg);
	camera_offset(&avg, &level->cam);
	camera_offset(&x, &level->cam);
	camera_offset(&y, &level->cam);
	camera_offset(&z, &level->cam);
	gizmo_print_line(avg, x, 0xff0000ff, pixels);
	gizmo_print_line(avg, y, 0x00ff00ff, pixels);
	gizmo_print_line(avg, z, 0x0000ffff, pixels);
	put_gizmo_box(x, 0xff0000ff, pixels);
	put_gizmo_box(y, 0x00ff00ff, pixels);
	put_gizmo_box(z, 0x0000ffff, pixels);
}

int				is_near(int a, int b, int range)
{
	float diff;

	diff = a - b;
	return (-range <= diff && diff <= range);
}

static t_vec3	calc_move_screen_space(int index, int amount)
{
	int		get_dir[3] = {0, 0, 0};
	t_vec3	dir;

	get_dir[index] = -1;
	dir.x = get_dir[0];
	dir.y = get_dir[1];
	dir.z = get_dir[2];
	vec_mult(&dir, amount / 100.);
	return (dir);
}

void			gizmo(t_level *level)
{
	t_vec3	x = {-1, 0, 0};
	t_vec3	y = {0, -1, 0};
	t_vec3	z = {0, 0, -1};

	t_vec3 avg = level->ui.state.gizmo_pos;
	float dist_from_screen = scale_translation_gizmo(&x, &y, &z, vec_sub_return(avg, level->cam.pos));
	vec_add(&x, x, avg);
	vec_add(&y, y, avg);
	vec_add(&z, z, avg);
	camera_offset(&avg, &level->cam);
	camera_offset(&x, &level->cam);
	camera_offset(&y, &level->cam);
	camera_offset(&z, &level->cam);

	int			mx;
	int			my;
	static int	drag_direction = 1;

	SDL_GetMouseState(&mx, &my);
	if (level->ui.state.m1_click)
	{
		if (x.z > 0 && is_near(mx, x.x , 9) && is_near(my, x.y, 9))
		{
			level->ui.state.mouse_location = MOUSE_LOCATION_GIZMO_X;
			drag_direction = vec_dot((t_vec3){0, 0, -1}, level->cam.front) > 0 ? -1 : 1;
		}
		else if (y.z > 0 && is_near(mx, y.x , 9) && is_near(my, y.y, 9))
		{
			level->ui.state.mouse_location = MOUSE_LOCATION_GIZMO_Y;
			drag_direction = 1;
		}
		else if (z.z > 0 && is_near(mx, z.x , 9) && is_near(my, z.y, 9))
		{
			level->ui.state.mouse_location = MOUSE_LOCATION_GIZMO_Z;
			drag_direction = vec_dot((t_vec3){-1, 0, 0}, level->cam.front) > 0 ? 1 : -1;
		}
	}
	if (!level->ui.state.m1_drag && level->ui.state.mouse_location >= MOUSE_LOCATION_GIZMO_X)
		level->ui.state.mouse_location = MOUSE_LOCATION_SELECTION;
	static int prevx = 0;
	static int prevy = 0;
	int deltax = prevx - mx;
	int deltay = prevy - my;
	t_vec3 res = {0, 0, 0};
	if (deltax || deltay)
	{
		if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_Y)
			res = calc_move_screen_space(level->ui.state.mouse_location - 3, deltay * dist_from_screen * drag_direction);
		else if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_X || level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_Z)
			res = calc_move_screen_space(level->ui.state.mouse_location - 3, deltax * dist_from_screen * drag_direction);
	}

	if (level->ui.state.ui_location == UI_LOCATION_DOOR_ACTIVATION_BUTTON)
		door_activation_move(level, res);
	else if (level->ui.state.ui_location == UI_LOCATION_LIGHT_EDITOR)
		move_light(level, res);
	else
		obj_editor_input(level, res);
	prevx = mx;
	prevy = my;
}

void			gizmo_render(t_level *level, t_window *window)
{
	static SDL_Texture	*texture = NULL;
	static unsigned		*pixels;
	signed				width;

	if (!texture)
	{
		texture = SDL_CreateTexture(window->SDLrenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, RES_X, RES_Y);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width))
			ft_error("failed to lock texture\n");
	}
	render_translation_gizmo(level, pixels);
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(window->SDLrenderer, texture, NULL, NULL);
	if (SDL_LockTexture(texture, NULL, (void**)&pixels, &width))
		ft_error("failed to lock texture\n");
	ft_memset(pixels, 0, RES_X * RES_Y * 4);
}