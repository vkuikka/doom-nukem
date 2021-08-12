/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/17 19:53:44 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/01 18:25:51 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	negative_check(const char *str, size_t *i)
{
	int	negative;

	negative = 1;
	if (str[*i] == '+' || str[*i] == '-')
	{
		if (str[*i] == '-')
			negative = -1;
		else
			negative = 1;
		*i = *i + 1;
	}
	if (str[*i] < '0' || str[*i] > '9')
		negative = 0;
	return (negative);
}

int	ft_atoi(const char *str)
{
	size_t		i;
	size_t		num;
	size_t		mult;
	int			negative;

	i = 0;
	mult = 1;
	num = 0;
	while (str[i] && (str[i] == ' ' || str[i] == '\v' || str[i] == '\t'
			|| str[i] == '\n' || str[i] == '\f' || str[i] == '\r'))
		i++;
	negative = negative_check(str, &i);
	while (str[i] >= '0' && str[i] <= '9')
		i++;
	i--;
	while ((str[i] >= '0' && str[i] <= '9'))
	{
		num += (str[i] - '0') * mult;
		mult *= 10;
		i--;
	}
	return (num * negative);
}
