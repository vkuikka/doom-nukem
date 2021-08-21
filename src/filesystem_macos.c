/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_macos.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/21 04:01:29 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/21 05:00:25 by rpehkone         ###   ########.fr       */
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

void	go_in_dir(char *path, char *folder)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (path[i])
		i++;
	path[i] = '/';
	i++;
	while (folder[j])
	{
		path[i] = folder[j];
		i++;
		j++;
	}
	path[i] = '\0';
}

void	path_up_dir(char *path)
{
	int	i;

	i = 0;
	while (path[i])
		i++;
	while (i && path[i] != '/')
		i--;
	if (i)
		path[i] = '\0';
}
