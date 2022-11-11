/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_unsigned.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 22:03:53 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/08 22:04:29 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

static int	ft_uinteger_2(unsigned long long num, t_nums info, int len)
{
	if (info.filler == ' ' && info.prefix)
		printf_write(NULL, &info.prefix, 1);
	if (info.width_pos == 1 && info.prefix == ' '
		&& info.width > info.precision && info.width > len)
		info.prefix = 0;
	if (info.precision > len)
		ft_putnchars('0', info.precision - len);
	if (num || (info.precision && !num))
		ft_putnbr_ull(num);
	if (info.precision > len)
		len = info.precision;
	if (info.width_pos == -1)
		ft_putnchars(' ', info.width - len - !!(info.prefix));
	return (len);
}

int	ft_uinteger(unsigned long long num, t_nums info)
{
	int		len;

	len = ft_unsignedlen(num);
	if (!num && !info.precision)
		len = 0;
	info.prefix = 0;
	if (info.precision > 0)
		info.filler = ' ';
	if (info.filler != ' ' && info.prefix)
		printf_write(NULL, &info.prefix, 1);
	if (info.width_pos == 1)
	{
		if (info.precision > len)
			ft_putnchars(info.filler,
				info.width - info.precision - !!(info.prefix));
		else
			ft_putnchars(info.filler, info.width - len - !!(info.prefix));
	}
	len = ft_uinteger_2(num, info, len);
	if ((len + !!(info.prefix)) < info.width)
		return (info.width);
	else
		return (len + !!(info.prefix));
}

static int	ft_integer_2(long long num, t_nums info, int len)
{
	if (info.width_pos == 1)
	{
		if (info.precision > len)
			ft_putnchars(info.filler,
				info.width - info.precision - !!(info.prefix));
		else
			ft_putnchars(info.filler, info.width - len - !!(info.prefix));
	}
	if (info.filler == ' ' && info.prefix)
		printf_write(NULL, &info.prefix, 1);
	if (info.width_pos == 1 && info.prefix == ' '
		&& info.width > info.precision && info.width > len)
		info.prefix = 0;
	if (info.precision > len)
		ft_putnchars('0', info.precision - len);
	if (num || (info.precision && !num))
		ft_putnbr_ull(num);
	if (info.precision > len)
		len = info.precision;
	if (info.width_pos == -1)
		ft_putnchars(' ', info.width - len - !!(info.prefix));
	return (len);
}

int	ft_integer(long long num, t_nums info)
{
	int		len;

	if (num < 0)
		len = ft_unsignedlen(num * -1);
	else
		len = ft_unsignedlen(num);
	if (!num && !info.precision)
		len = 0;
	if (info.prefix == '#')
		info.prefix = 0;
	if (num < 0)
	{
		info.prefix = '-';
		num *= -1;
	}
	if (info.precision > 0)
		info.filler = ' ';
	if (info.filler != ' ' && info.prefix)
		printf_write(NULL, &info.prefix, 1);
	len = ft_integer_2(num, info, len);
	if ((len + !!(info.prefix)) < info.width)
		return (info.width);
	else
		return (len + !!(info.prefix));
}
