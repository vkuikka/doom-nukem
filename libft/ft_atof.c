/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/01 01:01:01 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/01 18:10:45 by vkuikka          ###   ########.fr       */
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

double	ft_atof(const char *str)
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
		nbr = nbr * 10.0 + (*(str++) - '0');
	if (*str == '.')
	{
		str++;
		while (ft_isdigit(*str) && (++digits) && (++decimals))
		{
			nbr = nbr * 10.0 + (*str - '0');
			str++;
		}
	}
	while (decimals--)
		nbr /= 10.0;
	if (negative)
		return (-nbr);
	return (nbr);
}
