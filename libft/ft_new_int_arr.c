/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_new_int_arr.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/02 22:29:06 by vkuikka           #+#    #+#             */
/*   Updated: 2019/12/02 22:33:03 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	**ft_new_int_arr(unsigned int str_len, unsigned int arr_len)
{
	int				**ret;
	unsigned int	i;

	i = 0;
	ret = (int **)malloc(sizeof(int *) * arr_len);
	if (!ret)
		return (NULL);
	while (i < arr_len)
	{
		ret[i] = (int *)malloc(sizeof(int) * str_len);
		if (!ret[i])
			return (NULL);
		ft_bzero(ret[i], str_len);
		i++;
	}
	return (ret);
}
