/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deserialize_primitive.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 15:35:59 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/15 15:45:51 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

// Gcc assumes that your program will never access
// variables though pointers of different type.
// This assumption is called strict-aliasing and allows
// the compiler to make some optimizations.
// Strict-aliasing rule says that a char* and
// void* can point at any type.
static float	ntoh_float(float value)
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
