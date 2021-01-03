/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_quicksort.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/05 19:22:36 by vkuikka           #+#    #+#             */
/*   Updated: 2020/08/05 19:28:00 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_quicksort(int *nums, int index, int len)
{
	int		i;
	int		j;
	int		pivot;

	i = 0;
	j = 0;
	pivot = 0;
	if (index < len)
	{
		pivot = index;
		i = index;
		j = len;
		while (i < j)
		{
			while (nums[i] <= nums[pivot] && i < len)
				i++;
			while (nums[j] > nums[pivot])
				j--;
			if (i < j)
				ft_swap(&nums[i], &nums[j]);
		}
		ft_swap(&nums[pivot], &nums[j]);
		ft_quicksort(nums, index, j - 1);
		ft_quicksort(nums, j + 1, len);
	}
}
