/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_macos.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/21 04:01:29 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/21 04:30:55 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

char	*get_current_directory(void)
{
	int		path_max_size;
	char	*str;

	str = (char *)malloc(sizeof(char) * PATH_MAX);
	if (!str)
		ft_error("memory allocation failed\n");
	path_max_size = PATH_MAX - 2;
	_NSGetExecutablePath(str, &path_max_size);
	path_up_dir(str);
	path_up_dir(str);
	return (str);
}
