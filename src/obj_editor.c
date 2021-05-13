/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_editor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/22 23:13:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/22 23:13:42 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void		obj_pixel_put(int x, int y, int color, unsigned *texture)
{
	int oppacity;

	if (x < 0 || y < 0 || x > RES_X || y >= RES_Y)
		return;
	texture[x + (y * RES_X)] = color;
}

void	obj_print_line(t_vec3 start, t_vec3 stop, int color, unsigned *pixels)
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
		obj_pixel_put(pos.x, pos.y, color, pixels);
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
			obj_pixel_put(vertex.x + a, vertex.y + b, color, pixels);
			b++;
		}
		a++;
	}
}

static void		render_translation_gizmo(t_level *level, unsigned *pixels)
{
	t_vec3	avg = {0, 0, 0};
	t_vec3	x = {-1, 0, 0};
	t_vec3	y = {0, -1, 0};
	t_vec3	z = {0, 0, -1};
	int		selected_vert_amount;

	selected_vert_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
		for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
			if (level->all.tris[i].selected || (level->ui.vertex_select_mode && level->all.tris[i].verts[k].selected))
			{
				vec_add(&avg, avg, level->all.tris[i].verts[k].pos);
				selected_vert_amount++;
			}
	vec_div(&avg, selected_vert_amount);
	scale_translation_gizmo(&x, &y, &z, vec_sub_return(avg, level->cam.pos));
	vec_add(&x, x, avg);
	vec_add(&y, y, avg);
	vec_add(&z, z, avg);
	camera_offset(&avg, &level->cam);
	camera_offset(&x, &level->cam);
	camera_offset(&y, &level->cam);
	camera_offset(&z, &level->cam);
	obj_print_line(avg, x, 0xff0000ff, pixels);
	obj_print_line(avg, y, 0x00ff00ff, pixels);
	obj_print_line(avg, z, 0x0000ffff, pixels);
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

void			transform_quad(t_tri *tri, t_vec3 dir)
{
	for (int k = 0; k < 3 + tri->isquad; k++)
	{
		int quadmoved = 0;
		if (k == 3 && quadmoved)
			return ;
		if (tri->verts[k].selected)
		{
			vec_add(&tri->verts[k].pos, tri->verts[k].pos, dir);
			if (k < 3 && tri->isquad)
			{
				set_fourth_vertex(tri);
				quadmoved = 1;
			}
			else if (k == 3)
			{
				tri->verts[0].pos = tri->verts[3].pos;
				set_fourth_vertex(tri);
				t_vec3 tmp = tri->verts[0].pos;
				tri->verts[0].pos = tri->verts[3].pos;
				tri->verts[3].pos = tmp;
			}
		}
	}
}

void			move_selected(t_level *level, t_vec3 dir)
{
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		int amount = 0;
		for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
			if (level->all.tris[i].verts[k].selected)
				amount++;
		if (level->ui.vertex_select_mode && level->all.tris[i].isquad && amount != 4)
			transform_quad(&level->all.tris[i], dir);
		else
		{
			for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
				if (level->all.tris[i].selected || (level->ui.vertex_select_mode && level->all.tris[i].verts[k].selected))
				vec_add(&level->all.tris[i].verts[k].pos, level->all.tris[i].verts[k].pos, dir);
		}
		vec_sub(&level->all.tris[i].v0v2, level->all.tris[i].verts[1].pos, level->all.tris[i].verts[0].pos);
		vec_sub(&level->all.tris[i].v0v1, level->all.tris[i].verts[2].pos, level->all.tris[i].verts[0].pos);
	}
}

static t_vec3	calc_move_screen_space(int index, int amount, t_vec3 ss_gizmo)
{
	int		get_dir[3] = {0, 0, 0};
	t_vec3	dir;

	get_dir[index] = -1;
	dir.x = get_dir[0];
	dir.y = get_dir[1];
	dir.z = get_dir[2];
	t_vec3 vert;
	ss_gizmo.z = 0;
	vec_mult(&dir, amount / 100.);
	return (dir);
}

void				obj_editor_input(t_level *level)
{
	t_vec3	avg = {0, 0, 0};
	t_vec3	x = {-1, 0, 0};
	t_vec3	y = {0, -1, 0};
	t_vec3	z = {0, 0, -1};
	int		selected_vert_amount;

	selected_vert_amount = 0;
	for (int i = 0; i < level->all.tri_amount; i++)
		for (int k = 0; k < 3 + level->all.tris[i].isquad; k++)
			if (level->all.tris[i].selected || (level->ui.vertex_select_mode && level->all.tris[i].verts[k].selected))
			{
				vec_add(&avg, avg, level->all.tris[i].verts[k].pos);
				selected_vert_amount++;
			}
	vec_div(&avg, selected_vert_amount);
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
	if (deltax || deltay)
	{
		if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_X)
			move_selected(level, calc_move_screen_space(level->ui.state.mouse_location - 3, deltax * dist_from_screen * drag_direction, x));
		else if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_Y)
			move_selected(level, calc_move_screen_space(level->ui.state.mouse_location - 3, deltay * dist_from_screen * drag_direction, y));
		else if (level->ui.state.mouse_location == MOUSE_LOCATION_GIZMO_Z)
			move_selected(level, calc_move_screen_space(level->ui.state.mouse_location - 3, deltax * dist_from_screen * drag_direction, z));
	}
	prevx = mx;
	prevy = my;
}

void			obj_editor(t_level *level, t_window *window)
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

void			set_new_face_pos(t_obj *obj, int i, t_vec3 avg)
{
	obj->tris[i].verts[0].txtr.x = .5;
	obj->tris[i].verts[0].txtr.y = 0.;
	obj->tris[i].verts[1].txtr.x = 0.;
	obj->tris[i].verts[1].txtr.y = 1.;
	obj->tris[i].verts[2].txtr.x = 1.;
	obj->tris[i].verts[2].txtr.y = 1.;
	obj->tris[i].verts[0].pos = avg;
	obj->tris[i].verts[1].pos = avg;
	obj->tris[i].verts[2].pos = avg;

	obj->tris[i].verts[0].pos.y -= 2;
	obj->tris[i].verts[1].pos.y += 2;
	obj->tris[i].verts[2].pos.y += 2;
	obj->tris[i].verts[1].pos.x -= 2;
	obj->tris[i].verts[2].pos.x += 2;
}

void			set_new_face(t_level *level)
{
	int		i;
	t_vec3	tri_avg;

	i = level->all.tri_amount - 1;
	ft_bzero(&level->all.tris[i], sizeof(t_tri));
	tri_avg = level->cam.front;
	vec_mult(&tri_avg, 2);
	vec_add(&tri_avg, level->cam.pos, tri_avg);
	set_new_face_pos(&level->all, i, tri_avg);
	vec_sub(&level->all.tris[i].v0v2, level->all.tris[i].verts[1].pos, level->all.tris[i].verts[0].pos);
	vec_sub(&level->all.tris[i].v0v1, level->all.tris[i].verts[2].pos, level->all.tris[i].verts[0].pos);
	vec_cross(&level->all.tris[i].normal, level->all.tris[i].v0v1, level->all.tris[i].v0v2);
	vec_normalize(&level->all.tris[i].normal);
}

void			add_face(t_level *level)
{
	free_culling(level);
	level->all.tri_amount++;
	if (!(level->all.tris = (t_tri*)realloc(level->all.tris, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	if (!(level->visible.tris = (t_tri*)realloc(level->visible.tris, sizeof(t_tri) * level->all.tri_amount)))
		ft_error("memory allocation failed");
	set_new_face(level);
	init_screen_space_partition(level);
	init_culling(level);
}

void			remove_faces(t_level *level)
{
	int	amount;

	free_culling(level);
	amount = level->all.tri_amount;
	for (int i = 0; i < level->all.tri_amount; i++)
	{
		if (level->all.tris[i].selected)
		{
			for (int k = i; k < level->all.tri_amount - 1; k++)
				level->all.tris[k] = level->all.tris[k + 1];
			amount--;
		}
	}
	level->all.tri_amount = amount;
	if (!(level->all.tris = (t_tri*)realloc(level->all.tris, sizeof(t_tri) * amount)))
		ft_error("memory allocation failed");
	if (!(level->visible.tris = (t_tri*)realloc(level->visible.tris, sizeof(t_tri) * amount)))
		ft_error("memory allocation failed");
	init_screen_space_partition(level);
	init_culling(level);
}
