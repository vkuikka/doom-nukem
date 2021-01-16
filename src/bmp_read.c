/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bmp_read.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/08/07 18:28:42 by rpehkone          #+#    #+#             */
/*   Updated: 2021/01/03 03:51:15by rpehkone         ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

t_bmp	bmp_read(char *str)
{
	t_bmp_fileheader	fh;
	t_bmp_infoheader	ih;
	FILE				*img;
	int					x;
	int					y;
	t_bmp				res;

	img = fopen(str, "rb");
	fread(&fh, sizeof(unsigned char), sizeof(t_bmp_fileheader), img);
	fread(&ih, sizeof(unsigned char), sizeof(t_bmp_infoheader), img);
	//printf("fM1 = %c, fM2 = %c, bfS = %u, un1 = %hu, un2 = %hu, iDO = %u\n", fh.fileMarker1, fh.fileMarker2, fh.bfSize, fh.unused1, fh.unused2, fh.imageDataOffset);
	//printf("w = %d, h = %d, bits = %d\n", ih.width, ih.height, ih.bitPix);

	t_bmp_image bmp;
	res.image = (int*)malloc(sizeof(int) * (ih.width * ih.height));
	y = 0;
	while (y < ih.height)
	{
		x = 0;
		while (x < ih.width)
		{
			fread(&bmp, sizeof(unsigned char), sizeof(t_bmp_image), img);
			res.image[(ih.height - y) * ih.width + x] =
					(((int)(bmp.r) & 0xff) << 24) +
					(((int)(bmp.g) & 0xff) << 16) +
					(((int)(bmp.b) & 0xff) << 8) +
					((int)(bmp.a) & 0xff);
			x++;
		}
		y++;
	}
	fclose(img);
	res.width = ih.width;
	res.height = ih.height;
	return (res);
}
