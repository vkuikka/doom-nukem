/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bmp_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/18 00:32:28 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	set_24bit_rgb(t_bmp *res, FILE *img)
{
	t_bmp_pixel_24	px;
	int				x;
	int				y;

	y = 0;
	while (y < res->height)
	{
		x = 0;
		while (x < res->width)
		{
			fread(&px, sizeof(unsigned char), sizeof(t_bmp_pixel_24), img);
			res->image[(res->height - y - 1) *res->width + x]
				= (((int)(px.r) & 0xff) << 24) + (((int)(px.g) & 0xff) << 16)
				+ (((int)(px.b) & 0xff) << 8) + 0xff;
			x++;
		}
		y++;
	}
}

void	set_32bit_rgba(t_bmp *res, FILE *img)
{
	t_bmp_pixel_32	px;
	int				x;
	int				y;

	y = 0;
	while (y < res->height)
	{
		x = 0;
		while (x < res->width)
		{
			fread(&px, sizeof(unsigned char), sizeof(t_bmp_pixel_32), img);
			res->image[(res->height - y - 1) *res->width + x]
				= (((int)(px.r) & 0xff) << 24) + (((int)(px.g) & 0xff) << 16)
				+ (((int)(px.b) & 0xff) << 8) + ((int)(px.a) & 0xff);
			x++;
		}
		y++;
	}
}

t_bmp	bmp_read(char *filename)
{
	t_bmp_fileheader	fh;
	t_bmp_infoheader	ih;
	FILE				*img;
	t_bmp				res;

	img = fopen(filename, "rb");
	if (!img)
		ft_error("bmp read file reading failed\n");
	fread(&fh, sizeof(unsigned char), sizeof(t_bmp_fileheader), img);
	fread(&ih, sizeof(unsigned char), sizeof(t_bmp_infoheader), img);
	res.image = (int *)malloc(sizeof(int) * ih.width * ih.height);
	if (!res.image)
		ft_error("memory allocation failed\n");
	res.width = ih.width;
	res.height = ih.height;
	if (ih.bitPix == 32)
		set_32bit_rgba(&res, img);
	else if (ih.bitPix == 24)
		set_24bit_rgb(&res, img);
	else
		ft_error("bmp read Unknown bmp image bit depth\n");
	fclose(img);
	return (res);
}
