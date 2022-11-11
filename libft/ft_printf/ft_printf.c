/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 22:06:16 by vkuikka           #+#    #+#             */
/*   Updated: 2022/02/08 22:06:17 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printf.h"

int	ft_check_arg(char *arg)
{
	char	*tmp;

	tmp = arg;
	while (*tmp && (*tmp == '-' || *tmp == '+' || *tmp == ' '
			|| *tmp == '0' || *tmp == '#'))
		tmp++;
	while (*tmp && ft_isdigit(*tmp))
		tmp++;
	if (*tmp == '.' && *(tmp++))
		while (*tmp && ft_isdigit(*tmp))
			tmp++;
	if (*tmp == 'l' || *tmp == 'L')
		while (*tmp && (*tmp == 'l' || *tmp == 'L'))
			tmp++;
	else if (*tmp == 'h')
		while (*tmp && *tmp == 'h')
			tmp++;
	if (*tmp == 'd' || *tmp == 'i' || *tmp == 'o' || *tmp == 'u'
		|| *tmp == 'f' || *tmp == 'x' || *tmp == 'X' || *tmp == 'c'
		|| *tmp == 's' || *tmp == 'p' || *tmp == '%')
		return (1);
	return (0);
}

static t_nums	ft_info_init(void)
{
	t_nums	info;

	info.precision = -1;
	info.filler = ' ';
	info.prefix = 0;
	info.width = 0;
	info.width_pos = 1;
	info.valid = 1;
	info.intsize = 0;
	return (info);
}

static int	check_valid(char *arg)
{
	if (*arg == 'd' || *arg == 'i' || *arg == 'u'
		|| *arg == 'f' || *arg == 's' || *arg == 'c' || *arg == 'o')
		return (0);
	return (1);
}

int	ft_printarg(char *arg, va_list vl)
{
	t_nums	info;

	info = ft_info_init();
	while (info.valid && *arg)
	{
		if (*arg == '0' && info.precision == -1 && arg++)
			info.filler = '0';
		if (ft_isdigit(*arg) && info.precision == -1)
		{
			info.width = ft_atoi(arg);
			while (ft_isdigit(*arg))
				arg++;
		}
		if (*arg == 'd' || *arg == 'i' || *arg == 'u' || *arg == 'f')
			info.width = ft_nums(vl, info, *arg);
		else if ((*arg == 's' || *arg == 'c' || *arg == 'o'))
			info.width = ft_chars(vl, info, *arg);
		else
			info = ft_flags(arg, info);
		info.valid = check_valid(arg);
		if (info.valid && *arg)
			arg++;
	}
	return (info.width);
}

int	ft_sprintf(char *buff, char *arg, ...)
{
	va_list		vl;
	int			len;

	printf_write(buff, NULL, 0);
	len = 0;
	va_start(vl, arg);
	while (*arg)
	{
		if (*arg == '%' && *(++arg) && ft_check_arg(arg))
		{
			if (*arg == '%' && ++len)
				printf_write(NULL, &*arg, 1);
			else if (*arg)
				len += ft_printarg(arg, vl);
			arg = ft_skiparg(arg);
		}
		else if (*arg && ++len)
			printf_write(NULL, &*arg, 1);
		if (*arg)
			arg++;
	}
	va_end(vl);
	return (len);
}
