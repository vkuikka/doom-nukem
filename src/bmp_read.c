/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bmp_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/04/07 19:48:42 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

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
			res->image[(res->height - y - 1) * res->width + x] =
					(((int)(px.r) & 0xff) << 24) +
					(((int)(px.g) & 0xff) << 16) +
					(((int)(px.b) & 0xff) << 8) +
					0xff;
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
			res->image[(res->height - y - 1) * res->width + x] =
					(((int)(px.r) & 0xff) << 24) +
					(((int)(px.g) & 0xff) << 16) +
					(((int)(px.b) & 0xff) << 8) +
					((int)(px.a) & 0xff);
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

	global_seginfo = "bmp_read\n";
	if (!(img = fopen(filename, "rb")))
	{
		printf("%s", filename);
		ft_error(": no such file\n");
	}
	fread(&fh, sizeof(unsigned char), sizeof(t_bmp_fileheader), img);
	fread(&ih, sizeof(unsigned char), sizeof(t_bmp_infoheader), img);
	// printf("fM1 = %c, fM2 = %c, bfS = %u, un1 = %hu, un2 = %hu, iDO = %u\n", fh.fileMarker1, fh.fileMarker2, fh.bfSize, fh.unused1, fh.unused2, fh.imageDataOffset);
	// printf("w = %d, h = %d, bits = %d\n", ih.width, ih.height, ih.bitPix);

	if (!(res.image = (int*)malloc(sizeof(int) * ih.width * ih.height)))
		ft_error("memory allocation failed\n");
	res.width = ih.width;
	res.height = ih.height;
	if (ih.bitPix == 32)
		set_32bit_rgba(&res, img);
	else if (ih.bitPix == 24)
		set_24bit_rgb(&res, img);
	else
	{
		printf("%s", filename);
		ft_error(": Unknown bmp image bit depth\n");
	}
	fclose(img);
	return (res);
}
