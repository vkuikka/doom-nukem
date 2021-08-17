/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/20 10:54:29 by vkuikka           #+#    #+#             */
/*   Updated: 2021/06/20 11:43:10 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_realloc(void *ptr, size_t size, size_t new_size)
{
	void	*res;
	size_t	i;

	i = 0;
	res = (void *)malloc(new_size);
	if (!res)
		return (NULL);
	while (i < new_size)
	{
		if (i < size && ptr)
			((char *)res)[i] = ((char *)ptr)[i];
		else
			((char *)res)[i] = 0;
		i++;
	}
	if (ptr)
		free(ptr);
	return (res);
}
