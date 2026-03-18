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
	unsigned long	size;

	if (!getsectiondata(&_mh_execute_header, "embed", sector_name, &size))
		ft_error("failed to read embedded file size");
	return (size + 1);
}

unsigned char	*read_embedded_file(char *sector_name)
{
	unsigned long	size;
	unsigned char	*data;
	unsigned char	*buffer;

	data = getsectiondata(&_mh_execute_header, "embed", sector_name, &size);
	if (data == NULL)
		ft_error("failed to read embedded file");
	buffer = malloc(size + 1);
	ft_memcpy(buffer, data, size);
	buffer[size] = 0;
	return (buffer);
}
