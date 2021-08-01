/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/20 22:37:27 by vkuikka           #+#    #+#             */
/*   Updated: 2021/08/01 17:26:30 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_first_char(char *str, long *num, int chars)
{
	if (!str)
		return (0);
	str[chars--] = '\0';
	if (*num < 0)
		str[0] = '-';
	else
		str[0] = '0';
	if (*num < 0)
		*num *= -1;
	return (1);
}

char	*ft_itoa(int n)
{
	char			*str;
	int				chars;
	long			div;
	long			num;

	num = n;
	div = 10;
	chars = 1;
	if (num < 0)
		chars++;
	while (num / div && chars++ > 0)
		div *= 10;
	str = (char *)malloc(sizeof(char) * (chars + 1));
	if (!ft_first_char(str, &num, chars))
		return (NULL);
	while (chars >= (str[0] == '-'))
	{
		str[chars] = num % 10 + '0';
		num /= 10;
		chars--;
	}
	return (str);
}
