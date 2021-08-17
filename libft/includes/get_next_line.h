/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/17 14:44:12 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/01 19:44:12 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# define BUFF_SIZE 99
# define MAX_FD 10

# include <unistd.h>
# include <stdlib.h>
# include "libft.h"

int		get_next_line(const int fd, char **line);
int		first_checks(char **str, char **line, int fd, char *buff);
int		return_value(int read_ret, char **str, int fd, char **line);

#endif
