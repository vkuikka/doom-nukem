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

int	load_animation(char *filename, t_obj_animation *animation)
{
	int	i;

	animation->keyframe_amount = 3;
	animation->keyframes = malloc(sizeof(t_obj) * animation->keyframe_amount);
	i = 0;
	while (i < 3)
	{
		filename[ft_strlen(filename) - 4 = '0' + i;
		if (!load_obj(filename, &animation->keyframes[i]))
			return (FALSE);
		i++;
	}
	return (TRUE);
}

void	play_animation(t_obj *target, t_obj_animation *animation, unsigned int start_time)
{
	;
}
