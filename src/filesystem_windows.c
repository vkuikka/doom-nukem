/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_windows.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/21 04:01:26 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/21 22:19:17 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

char	*get_current_directory(void)
{
	TCHAR	szFileName[PATH_MAX];
	char	*str;

	str = (char *)malloc(sizeof(char) * PATH_MAX);
	if (!str)
		ft_error("memory allocation failed\n");
	GetModuleFileName(NULL, szFileName, PATH_MAX);
	ft_strcpy(str, szFileName);
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
	path[i] = '\\';
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
	while (i && path[i] != '\\')
		i--;
	if (i)
		path[i] = '\0';
}

void	loop_directory(char *directory, void *data,
					void (*f)(int, char *, void *))
{
	WIN32_FIND_DATA	data;
	HANDLE			dir;
	char			*dirname;

	dirname = ft_strjoin(directory, "\\*");
	dir = FindFirstFile(dirname, &data);
	if (dir == INVALID_HANDLE_VALUE)
		ft_error("Cannot open directory\n");
	while (FindNextFile(dir, &data))
	{
		if (data.cFileName[0] != '.')
		{
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY())
				(*f)(1, data.cFileName, data);
			else
				(*f)(0, data.cFileName, data);
		}
	}
	FindClose(dir);
	free(dirname);
}
