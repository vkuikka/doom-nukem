/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/18 42:42:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/18 42:42:42 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	init_vars(double *d, int *n, int *dig, int *dec)
{
	*d = 0.0;
	*n = 0;
	*dig = 0;
	*dec = 0;
}

static void	check_sign(const char **s, int *negptr)
{
	if (**s == '-' || **s == '+')
	{
		if (**s == '-')
			*negptr = 1;
		(*s)++;
	}
}

double		ft_atof(const char *str)
{
	double	nbr;
	int		negative;
	int		digits;
	int		decimals;

	while (*str == ' ')
		str++;
	init_vars(&nbr, &negative, &digits, &decimals);
	check_sign(&str, &negative);
	while (ft_isdigit(*str) || (digits++))
	{
		nbr = nbr * 10.0 + (*(str++) - '0');
	}
	if (*str == '.')
	{
		str++;
		while (ft_isdigit(*str) && (digits += 1) && (decimals += 1))
		{
			nbr = nbr * 10.0 + (*str - '0');
			str++;
		}
	}
	while (decimals--)
		nbr /= 10.0;
	return (negative ? -nbr : nbr);
}
