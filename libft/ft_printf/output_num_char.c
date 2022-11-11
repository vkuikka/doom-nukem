/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_num_char.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/29 17:50:22 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/08 22:06:35 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

static int	ft_float_2(long double num, t_nums info)
{
	int	len;

	if (info.width_pos == 1)
		ft_putnchars(info.filler, info.width);
	if (info.filler == ' ' && info.prefix && info.prefix != '#' && num > 0)
		printf_write(NULL, &info.prefix, 1);
	if (num < 0 && info.filler == ' ')
		printf_write(NULL, "-", 1);
	len = ft_putfloat(num, info.precision);
	if (!info.precision && info.prefix == '#' && ++len)
		printf_write(NULL, ".", 1);
	if (info.width_pos == -1)
		ft_putnchars(' ', info.width);
	return (len);
}

int	ft_float(long double num, t_nums info)
{
	int		len;

	if (info.precision == -1)
		info.precision = 6;
	if (num < 0 && info.prefix != '#')
		info.prefix = 0;
	info.width -= ft_float_len(num, info.precision)
		+ (num < 0 || (info.prefix && info.prefix != '#'));
	if (info.filler != ' ' && info.prefix && info.prefix != '#' && num > 0)
		printf_write(NULL, &info.prefix, 1);
	if (num < 0 && info.filler != ' ')
		printf_write(NULL, "-", 1);
	len = ft_float_2(num, info);
	if (info.width > 0)
		return (len + info.width
			+ (info.prefix && info.prefix != '#') + num > 0);
	else
		return (len + (info.prefix && info.prefix != '#') + num > 0);
}

static int	ft_string_2(char *s, long long c, t_nums info, int len)
{
	if (s)
		ft_putstr_len(s, len);
	else if (c != -1)
		printf_write(NULL, (char *)&c, 1);
	if (info.width_pos == -1)
		ft_putnchars(' ', info.width - len);
	if (len < info.width)
		len = info.width;
	return (len);
}

int	ft_string(char *s, long long c, t_nums info)
{
	int		len;

	if (!s && c == -1)
		s = "(null)";
	len = 1;
	if (s)
		len = ft_strlen(s);
	if (s && s[0] == '\0')
	{
		ft_putnchars(info.filler, info.width);
		return (info.width);
	}
	if (!s && c == -1)
		len = 6;
	if (info.precision > -1 && info.precision < len)
		len = info.precision;
	if (info.width_pos == 1)
		ft_putnchars(info.filler, info.width - len);
	len = ft_string_2(s, c, info, len);
	if (c == 0 && !len)
		return (1);
	return (len);
}

int	ft_percent(t_nums info)
{
	int	len;

	len = 1;
	if (info.width_pos == 1)
		ft_putnchars(info.filler, info.width - len);
	printf_write(NULL, "%", 1);
	if (info.width_pos == -1)
		ft_putnchars(' ', info.width - len);
	if (info.width)
		return (info.width);
	else
		return (1);
}
