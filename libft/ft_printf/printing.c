/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 17:59:41 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/08 21:28:12 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

void	printf_write(char *buff, char *str, size_t len)
{
	static char	*mem;
	static int	i;

	if (buff)
	{
		i = 0;
		mem = buff;
		ft_bzero(mem, 100);
		return ;
	}
	ft_strncpy(&(mem[i]), str, len);
	i += len;
	return ;
}

void	ft_putstr_len(char *str, size_t len)
{
	if (ft_strlen(str) > len)
		printf_write(NULL, str, (size_t)len);
	else
		printf_write(NULL, str, ft_strlen(str));
}

void	ft_putnchars(char c, int amount)
{
	while (amount > 0)
	{
		printf_write(NULL, &c, 1);
		amount--;
	}
}

void	ft_putnbr_ull(unsigned long long n)
{
	char	c;

	if (n > 9)
	{
		ft_putnbr_ull(n / 10);
		c = n % 10 + '0';
	}
	else
		c = n + '0';
	printf_write(NULL, &c, 1);
}
