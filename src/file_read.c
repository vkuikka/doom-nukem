/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_read.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/02 15:15:43 by rpehkone          #+#    #+#             */
/*   Updated: 2021/09/02 16:14:56 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	free_file2d(char **file)
{
	int	i;

	i = 0;
	while (file[i])
	{
		free(file[i]);
		i++;
	}
	free(file);
}

char	**file2d(char *filename)
{
	char	**file;
	char	*line;
	int		fd;
	int		i;

	i = 0;
	fd = open(filename, O_RDONLY);
	if (fd < 2)
		return (NULL);
	while (get_next_line(fd, &line))
	{
		free(line);
		i++;
	}
	free(line);
	close(fd);
	file = (char **)malloc(sizeof(char *) * (i + 1));
	if (!file)
		return (NULL);
	fd = open(filename, O_RDONLY);
	if (fd < 2)
		return (NULL);
	i = 0;
	while (get_next_line(fd, &line))
	{
		file[i] = line;
		i++;
	}
	free(line);
	file[i] = NULL;
	close(fd);
	return (file);
}

char	**file2d_from_memory(unsigned char *data, unsigned int size)
{
	unsigned int	i;
	unsigned int	k;
	unsigned int	len;
	char			**file;

	i = 0;
	k = 0;
	while (i < size)
	{
		if (data[i] == '\n')
			k++;
		i++;
	}
	file = (char **)malloc(sizeof(char *) * (k + 1));
	if (!file)
		ft_error("memory allocation failed");
	file[k] = NULL;
	i = 0;
	k = 0;
	len = 0;
	while (i < size)
	{
		if ((char)data[i] == '\n')
		{
			if (len)
				len--;
			file[k] = (char *)malloc(sizeof(char) * (len + 1));
			if (!file[k])
				ft_error("memory allocation failed");
			ft_strncpy(file[k], (char *)&data[i - len], len);
			file[k][len] = '\0';
			len = 0;
			k++;
		}
		len++;
		i++;
	}
	return (file);
}

