/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_add_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/08 15:13:27 by vkuikka           #+#    #+#             */
/*   Updated: 2020/09/08 15:13:28by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	**ft_add_str(char *str, char **arr, unsigned arrlen)
{
	char		**new;
	unsigned	i;

	i = 0;
	if (!str)
		return (arr);
	if (!arr)
		arrlen = 0;
	while (i < arrlen && arr[i])
		i++;
	arrlen = i + 1;
	if (!(new = (char **)malloc(sizeof(char*) * arrlen)))
		return (NULL);
	i = 0;
	while (i < arrlen - 1)
	{
		new[i] = ft_strdup(arr[i]);
		i++;
	}
	new[i] = ft_strdup(str);
	return (new);
}
