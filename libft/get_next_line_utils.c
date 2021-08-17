/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/01 19:43:17 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/01 19:45:38 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_free_line(char *input)
{
	char	*cpy;
	int		i;

	i = 0;
	while (input[i] && input[i] != '\n')
		i++;
	if (input[i] == '\n')
		i++;
	cpy = ft_strdup(&input[i]);
	i = 0;
	while (cpy[i])
		i++;
	if (input && *input)
	{
		free(input);
		input = NULL;
	}
	return (cpy);
}

int	first_checks(char **str, char **line, int fd, char *buff)
{
	if (!line || fd < 0 || read(fd, buff, 0) < 0)
		return (0);
	if (!(str[fd]))
	{
		str[fd] = ft_strnew(BUFF_SIZE);
		if (!str[fd])
			return (0);
	}
	return (1);
}

int	return_value(int read_ret, char **str, int fd, char **line)
{
	if (!*line)
		read_ret = -1;
	if (*str[fd])
	{
		read_ret = 1;
		str[fd] = ft_free_line(str[fd]);
	}
	return (read_ret);
}
