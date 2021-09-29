/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   animation.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/22 17:08:10 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/22 17:08:10 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

int	load_animation(char *get_filename, t_obj_animation *animation, int amount)
{
	char	filename[100];
	int		res;
	int		i;

	res = TRUE;
	animation->keyframe_amount = amount;
	animation->keyframes = (t_obj *)malloc(sizeof(t_obj)
			* animation->keyframe_amount);
	i = 0;
	while (i < amount && res)
	{
		sprintf(filename, "%s_%06d.obj", get_filename, i);
		if (!load_obj(filename, &animation->keyframes[i]))
			res = FALSE;
		i++;
	}
	i = 1;
	while (i < amount && res)
	{
		if (animation->keyframes[0].tri_amount
			!= animation->keyframes[i].tri_amount)
			res = FALSE;
		i++;
	}
	if (!res)
	{
		;//free objs
	}
	return (res);
}

t_obj get_animation_target(t_obj_animation *animation)
{
	t_obj	res;

	res.tri_amount = animation->keyframes[0].tri_amount;
	res.tris = (t_tri *)malloc(sizeof(t_tri) * res.tri_amount);
	if (!res.tris)
		ft_error("animation malloc fail");
	return (res);
}

static void	tri_lerp(t_tri *target, t_tri *a, t_tri *b, float t)
{
	int	i;

	i = 0;
	*target = *a;
	while (i < 3 + target->isquad)
	{
		target->verts[i].pos
			= vec_interpolate(a->verts[i].pos, b->verts[i].pos, t);
		i++;
	}
}

void	obj_copy(t_obj *target, t_obj *source)
{
	int	i;

	i = 0;
	target->tri_amount = source->tri_amount;
	while (i < source->tri_amount)
	{
		target->tris[i] = source->tris[i];
		i++;
	}
}

void	play_animation(t_obj *target, t_obj_animation *animation,	
						unsigned int start_time, float anim_speed_modifier)
{
	float	time;
	float	node_time;
	float	elem_size;
	t_ivec2	id;
	int		i;

	if (!anim_speed_modifier)
		return ;
	time = (SDL_GetTicks() - start_time)
		/ (1000.0 * animation->duration / anim_speed_modifier);
	time = fmod(time, 1.0);
	id.x = ((int)((animation->keyframe_amount - 0) * time) + 0) % animation->keyframe_amount;
	id.y = ((int)((animation->keyframe_amount - 0) * time) + 1) % animation->keyframe_amount;
	elem_size = 1.0 / (animation->keyframe_amount - 0);
	node_time = (time - (elem_size * id.x)) / elem_size;
	i = 0;
	while (i < target->tri_amount)
	{
		tri_lerp(&target->tris[i], &animation->keyframes[id.x].tris[i],
				&animation->keyframes[id.y].tris[i], node_time);
		i++;
	}
}
