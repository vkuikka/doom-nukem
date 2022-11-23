/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deserialize_primitive_0.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

float	ntoh_float(float value)
{
	int		temp;
	void	*cast;

	cast = &value;
	temp = ntohl(*(unsigned int *)cast);
	cast = &temp;
	return (*(float *)cast);
}

void	deserialize_float(float *x, t_buffer *buf)
{
	if (buf->next + sizeof(float) > buf->size)
		ft_error("doom nukem file read fail");
	ft_memcpy(x, ((char *)buf->data) + buf->next, sizeof(float));
	*x = ntoh_float(*x);
	buf->next += sizeof(float);
}

void	deserialize_int(int *x, t_buffer *buf)
{
	if (buf->next + sizeof(int) > buf->size)
		ft_error("doom nukem file read fail");
	ft_memcpy(x, ((char *)buf->data) + buf->next, sizeof(int));
	*x = ntohl(*x);
	buf->next += sizeof(int);
}
