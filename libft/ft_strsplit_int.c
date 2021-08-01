/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsplit_int.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/08 13:25:36 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/01 18:26:49 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	*ft_strsplit_int(char *s, char c)
{
	int		i;
	int		*ret;

	i = 0;
	ret = (int *)malloc(sizeof(int) * ft_word_count(s, c));
	if (!ret)
		return (NULL);
	while (*s)
	{
		if (*s != c && *s)
		{
			ret[i] = ft_atoi(s);
			i++;
		}
		while (*s != c && *s)
			s++;
		while (*s == c && *s)
			s++;
	}
	return (ret);
}
