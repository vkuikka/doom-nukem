/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serialization.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 15:30:53 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/15 15:45:55 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERIALIZATION_H
# define SERIALIZATION_H
# define SERIALIZE_INITIAL_BUFFER_SIZE 512

# include "doom_nukem.h"

void	deserialize_int(int *x, t_buffer *buf);
void	deserialize_float(float *x, t_buffer *buf);

void	serialize_int(int x, t_buffer *buf);
void	serialize_float(float x, t_buffer *buf);

#endif