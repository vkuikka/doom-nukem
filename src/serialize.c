/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serialize.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/06 14:13:02 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/06 22:01:47 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

t_buffer	*new_buffer(void)
{
	t_buffer *buf = malloc(sizeof(t_buffer));

	if (!(buf->data = malloc(SERIALIZE_INITIAL_BUFFER_SIZE)))
		ft_error("memory allocation failed\n");
	buf->size = SERIALIZE_INITIAL_BUFFER_SIZE;
	buf->next = 0;
	return buf;
}

void	reserve_space(t_buffer *buf, size_t bytes)
{
	if ((buf->next + bytes) > buf->size)
	{
		if (!(buf->data = realloc(buf->data, buf->size * 2)))
			ft_error("memory allocation failed\n");
		buf->size *= 2;
	}
}

float	ntoh_float(float value)
{
	int temp;

	temp = ntohl(*(unsigned int*)&value);
	return *(float*)&temp;
}

void	deserialize_float(float *x, t_buffer *buf)
{
	memcpy(x, ((char *)buf->data) + buf->next, sizeof(float));
	*x = ntoh_float(*x);
	buf->next += sizeof(float);
}

float	hton_float(float value)
{
	int temp;

	temp = htonl(*(unsigned int*)&value);
	return *(float*)&temp;
}

void	serialize_float(float x, t_buffer *buf)
{
	x = hton_float(x);
	reserve_space(buf, sizeof(float));
	memcpy(((char *)buf->data) + buf->next, &x, sizeof(float));
	buf->next += sizeof(float);
}

void	deserialize_int(int *x, t_buffer *buf)
{
	memcpy(x, ((char *)buf->data) + buf->next, sizeof(int));
	*x = ntohl(*x);
	buf->next += sizeof(int);
}

void	serialize_int(int x, t_buffer *buf)
{
	x = htonl(x);
	reserve_space(buf, sizeof(int));
	memcpy(((char *)buf->data) + buf->next, &x, sizeof(int));
	buf->next += sizeof(int);
}

void	deserialize_settings(t_level *level, t_buffer *buf)
{
	deserialize_int(&level->ui->fog, buf);
	deserialize_int((int*)&level->ui->fog_color, buf);
	deserialize_int(&level->ui->backface_culling, buf);
	deserialize_int(&level->ui->distance_culling, buf);
	deserialize_float(&level->ui->render_distance, buf);
	deserialize_float(&level->ui->sun_contrast, buf);
	deserialize_float(&level->ui->direct_shadow_contrast, buf);
	// deserialize_vec3(level->ui->sun_dir, output);
}

void	serialize_settings(t_level *level, t_buffer *buf)
{
	serialize_int(level->ui->fog, buf);
	serialize_int(level->ui->fog_color, buf);
	serialize_int(level->ui->backface_culling, buf);
	serialize_int(level->ui->distance_culling, buf);
	serialize_float(level->ui->render_distance, buf);
	serialize_float(level->ui->sun_contrast, buf);
	serialize_float(level->ui->direct_shadow_contrast, buf);
	// serialize_vec3(level->ui->sun_dir, output);
}

void	deserialize_level(t_level *level, t_buffer *buf)
{
	deserialize_settings(level, buf);
	// deserialize_obj(level->all);
	// deserialize_bmp(level->texture);
	//trim max size
}

void	serialize_level(t_level *level, t_buffer *buf)
{
	serialize_settings(level, buf);
	// serialize_obj(level->all);
	// serialize_bmp(level->texture);
	//trim max size
}

#ifdef _WIN32
void	save_file(t_level *level, t_buffer *buf)
{
	HANDLE	hFile;
	char	*filename1;
	char	*filename2;
	DWORD	bytesWritten;

	filename1 = ft_strjoin(level->ui->state.directory, "\\");
	filename2 = ft_strjoin(filename1, level->ui->state.save_filename);
	free(filename1);
	filename1 = ft_strjoin(filename2, ".doom-nukem");
	free(filename2);
	hFile = CreateFile(filename1, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		ft_error("failed to create file");
	WriteFile(hFile, buf->data, buf->size, &bytesWritten, NULL);
	free(filename1);
	CloseHandle(hFile);
}

void	open_file(char *filename, t_buffer *buf)
{
	HANDLE	hFile;
	DWORD	nRead;

	hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		ft_error("failed to read file");
	size_t file_size = (size_t)GetFileSize(hFile, NULL);
	if (!(buf->data = malloc(file_size + 1)))
		ft_error("failed to allocate memory for file");
	if (!ReadFile(hFile, buf->data, file_size, &nRead, NULL))
		ft_error("failed to read file");
	buf->size = nRead;
	CloseHandle(hFile);
}

#elif __APPLE__
void	save_file(t_level *level, t_buffer *buf)
{
	int		fd;
	char	*filename1;
	char	*filename2;

	filename1 = ft_strjoin(level->ui->state.directory, "/");
	filename2 = ft_strjoin(filename1, level->ui->state.save_filename);
	free(filename1);
	filename1 = ft_strjoin(filename2, ".doom-nukem");
	free(filename2);
	fd = open(filename1, O_WRONLY | O_CREAT);
	if (fd < 3)
		ft_error("failed to write file");
	if (write(fd, buf->data, buf->size) != buf->size)
		ft_error("failed to write file");
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
	if (!(buf->data = malloc(buf->size + 1)))
		ft_error("failed to allocate memory for file");
	if (read(fd, buf->data, buf->size) != buf->size)
		ft_error("failed to read file");
	close(fd);
}
#endif

void	save_level(t_level *level)
{
	t_buffer *buf = new_buffer();
	serialize_level(level, buf);
	save_file(level, buf);
	free(buf);
}

void	open_level(t_level *level, char *filename)
{
	t_buffer	buf;

	buf.data = 0;
	buf.next = 0;
	buf.size = 0;
	open_file(filename, &buf);
	//free level things
	deserialize_level(level, &buf);
	free(buf.data);
}