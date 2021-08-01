/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/03 13:01:40 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/01 19:45:30 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_strdup_nl(char *src)
{
	int		i;
	int		count;
	char	*str;

	count = 0;
	i = 0;
	while (src && src[i] && src[i] != '\n')
		i++;
	str = (char *)malloc(sizeof(str) * (i + 1));
	if (!str)
		return (NULL);
	while (count < i && src[count] && src[count] != '\n')
	{
		str[count] = src[count];
		count++;
	}
	str[i] = '\0';
	return (str);
}

static char	*ft_strjoin_free(char *s1, char const *s2, int stop_nl)
{
	char	*str;
	int		len_1;
	int		len_2;

	len_1 = 0;
	len_2 = 0;
	while (s1 && *s1 && s1[len_1] && (s1[len_1] != '\n' || !stop_nl))
		len_1++;
	while (s2[len_2] && (s2[len_2] != '\n' || !stop_nl))
		len_2++;
	str = (char *)malloc(len_1 + len_2 + 1);
	if (!str)
		return (NULL);
	len_1 = -1;
	if (s1 && !*s1)
		len_1 = 0;
	len_2 = 0;
	if (s1 && *s1)
		while (s1[++len_1])
			str[len_1] = s1[len_1];
	while (s2[len_2] && (s2[len_2] != '\n' || !stop_nl))
		str[len_1++] = s2[len_2++];
	str[len_1] = '\0';
	free(s1);
	return (str);
}

int	get_next_line(const int fd, char **line)
{
	static char		*str[MAX_FD];
	char			buff[BUFF_SIZE + 1];
	int				read_ret;
	int				i;

	i = 0;
	read_ret = 1;
	if (!first_checks(str, line, fd, buff))
		return (-1);
	while (str[fd][i] && str[fd][i] != '\n')
		i++;
	while (str[fd][i] != '\n' && read_ret > 0)
	{
		read_ret = read(fd, buff, BUFF_SIZE);
		buff[read_ret] = '\0';
		str[fd] = ft_strjoin_free(str[fd], buff, 0);
		i = 0;
		while (str[fd][i] && str[fd][i] != '\n')
			i++;
	}
	*line = ft_strdup_nl(str[fd]);
	return (return_value(read_ret, str, fd, line));
}
