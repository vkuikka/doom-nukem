/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_macos_0.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/05 22:24:15 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

char	*get_current_directory(void)
{
	uint32_t	path_max_size;
	char		*str;
	char		*abs;

	str = (char *)malloc(sizeof(char) * PATH_MAX);
	if (!str)
		ft_error("memory allocation failed\n");
	path_max_size = PATH_MAX - 2;
	_NSGetExecutablePath(str, &path_max_size);
	abs = realpath(str, NULL);
	free(str);
	str = abs;
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

void	loop_directory(char *directory, void *data,
					void (*f)(int, char *, void *))
{
	DIR				*dir;
	struct dirent	*ent;

	dir = opendir(directory);
	if (!dir)
		ft_error("Cannot open directory\n");
	ent = readdir(dir);
	while (ent != NULL)
	{
		if (ent->d_name[0] != '.')
		{
			if (ent->d_type == DT_DIR)
				(*f)(1, ent->d_name, data);
			else if (ent->d_type == DT_REG)
				(*f)(0, ent->d_name, data);
		}
		ent = readdir(dir);
	}
	closedir(dir);
}
