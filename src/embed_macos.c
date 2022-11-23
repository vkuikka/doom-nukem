/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   embed_macos.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/05 22:24:01 by rpehkone          #+#    #+#             */
/*   Updated: 2022/04/02 00:43:57 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

unsigned long	get_embedded_size(char *sector_name)
{
	const struct section_64	*sect;

	sect = getsectbyname("embed", sector_name);
	if (sect == NULL)
		ft_error("failed to read embedded file size");
	return ((unsigned long)sect->size + 1);
}

unsigned char	*read_embedded_file(char *sector_name)
{
	const struct section_64	*sect;
	unsigned char			*buffer;
	int						fd;

	sect = getsectbyname("embed", sector_name);
	if (sect == NULL)
		ft_error("failed to read embedded file");
	buffer = malloc(sect->size + 1);
	ft_bzero(buffer, sect->size + 1);
	fd = open("doom-nukem", O_RDONLY);
	if (fd < 0)
		ft_error("failed to read embedded file");
	lseek(fd, sect->offset, SEEK_SET);
	if (read(fd, buffer, sect->size) != (ssize_t)sect->size)
	{
		close(fd);
		ft_error("failed to read embedded file");
	}
	close(fd);
	return (buffer);
}
