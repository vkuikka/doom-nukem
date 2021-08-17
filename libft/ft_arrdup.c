/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_arrdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 19:20:35 by vkuikka           #+#    #+#             */
/*   Updated: 2020/08/05 19:29:55 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	*ft_arrdup(const int *src, const int len)
{
	int		i;
	int		*arr;

	i = -1;
	arr = (int *)malloc(sizeof(int) * len);
	if (!arr)
		return (NULL);
	while (++i < len)
		arr[i] = src[i];
	return (arr);
}
