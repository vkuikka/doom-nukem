/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_read_0.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/07 14:07:00 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	free_file2d(char **file)
{
	free(file[0]);
	free(file);
}

char	**file2d_internal(char *file, long size)
{
	int		i;
	int		line_count;
	char	**res;

	line_count = 0;
	i = -1;
	while (++i < size)
		if (file[i] == '\n')
			line_count++;
	res = (char **)malloc(sizeof(char *) * (line_count + 1));
	line_count = 0;
	res[line_count] = &file[0];
	i = -1;
	while (++i < size)
	{
		if (file[i] == '\n')
		{
			file[i] = '\0';
			res[++line_count] = &file[i + 1];
		}
	}
	res[line_count] = NULL;
	return (res);
}

char	**file2d(char *filename)
{
	char	*file;
	int		fd;
	long	size;

	fd = open(filename, O_RDONLY);
	if (fd < 2)
		return (NULL);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	file = (char *)malloc(sizeof(char) * (size + 1));
	size = read(fd, file, size);
	file[size] = '\0';
	close(fd);
	return (file2d_internal(file, size));
}

char	**file2d_from_memory(unsigned char *data)
{
	size_t	size;
	char	*file;

	size = ft_strlen((char *)data);
	file = (char *)malloc(sizeof(char) * (size));
	memcpy(file, data, size);
	return (file2d_internal(file, (long)size));
}
