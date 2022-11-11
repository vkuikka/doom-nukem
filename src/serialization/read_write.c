/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_write_0.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/02 15:28:13 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	save_file(t_level *level, t_buffer *buf)
{
	int		fd;
	char	*filename1;
	char	*filename2;

	filename1 = ft_strjoin(level->ui.state.directory, "/");
	filename2 = ft_strjoin(filename1, level->ui.state.save_filename);
	free(filename1);
	filename1 = ft_strjoin(filename2, ".doom-nukem");
	free(filename2);
	fd = open(filename1, O_RDWR | O_CREAT,
			S_IRGRP | S_IWGRP | S_IXGRP
			| S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd < 3)
		nonfatal_error("failed to create file");
	else if (write(fd, buf->data, buf->next) != buf->next)
		nonfatal_error("failed to write file");
	free(filename1);
	close(fd);
}

void	open_file(char *filename, t_buffer *buf)
{
	struct stat	fstat;
	int			fd;

	fd = open(filename, O_RDONLY);
	if (fd < 3)
		ft_error("failed to read file");
	stat(filename, &fstat);
	buf->size = fstat.st_size;
	buf->data = malloc(buf->size + 1);
	if (!buf->data)
		ft_error("failed to allocate memory for file");
	if ((size_t)read(fd, buf->data, buf->size) != buf->size)
		ft_error("failed to read file");
	close(fd);
}

void	save_level(t_level *level)
{
	t_buffer	buf;

	if (!level->ui.state.save_filename[0]
		|| level->ui.state.save_filename[0] == '.')
	{
		nonfatal_error("Invalid filename");
		return ;
	}
	buf.data = malloc(SERIALIZE_INITIAL_BUFFER_SIZE);
	if (!buf.data)
		ft_error("memory allocation failed\n");
	buf.size = SERIALIZE_INITIAL_BUFFER_SIZE;
	buf.next = 0;
	serialize_level(level, &buf);
	save_file(level, &buf);
}

void	open_level(t_level *level, char *filename)
{
	t_buffer	buf;

	buf.data = 0;
	buf.next = 0;
	buf.size = 0;
	open_file(filename, &buf);
	deserialize_level(level, &buf);
	free(buf.data);
}
