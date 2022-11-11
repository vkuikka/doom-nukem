/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bmp_read_0.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/04 16:54:13 by vkuikka           #+#    #+#             */
/*   Updated: 2021/10/11 18:58:59 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

void	bmp_set(t_bmp *img, int *tmp)
{
	unsigned char	*rgb;
	int				x;
	int				y;
	int				c;

	y = 0;
	while (y < img->height)
	{
		x = 0;
		while (x < img->width)
		{
			rgb = (unsigned char *)&tmp[((img->height - y - 1) * img->width)
				+ x];
			c = (rgb[0] << 8 * 1)
				+ (rgb[1] << 8 * 2)
				+ (rgb[2] << 8 * 3)
				+ (rgb[3] << 8 * 0);
			img->image[y * img->width + x] = c;
			x++;
		}
		y++;
	}
}

int	bmp_error_check(t_bmp_fileheader fh, t_bmp_infoheader ih)
{
	if (fh.fileMarker1 != 'B' && fh.fileMarker2 != 'M')
		nonfatal_error("not bmp file");
	else if (ih.bitPix != 32)
		nonfatal_error("bmp read Unknown bmp image bit depth\n");
	else if (ih.biCompression != 0)
		nonfatal_error("invalid bmp settings (use provided ./texture.py)");
	else
		return (FALSE);
	return (TRUE);
}

t_bmp	bmp_read(char *filename)
{
	t_bmp_fileheader	fh;
	t_bmp_infoheader	ih;
	FILE				*img;
	t_bmp				res;
	int					*tmp;

	img = fopen(filename, "rb");
	if (!img)
		ft_error("bmp read file reading failed\n");
	fread(&fh, sizeof(unsigned char), sizeof(t_bmp_fileheader), img);
	fread(&ih, sizeof(unsigned char), sizeof(t_bmp_infoheader), img);
	ft_bzero(&res, sizeof(t_bmp));
	if (bmp_error_check(fh, ih))
		return (res);
	res.image = (int *)malloc(sizeof(int) * ih.width * ih.height);
	tmp = (int *)malloc(sizeof(int) * ih.width * ih.height);
	if (!res.image || !tmp)
		ft_error("bmp memory allocation failed\n");
	res.width = ih.width;
	res.height = ih.height;
	fread(tmp, sizeof(unsigned char), sizeof(int) * ih.width * ih.height, img);
	bmp_set(&res, tmp);
	free(tmp);
	fclose(img);
	return (res);
}

t_bmp	bmp_read_from_memory(unsigned char *data)
{
	t_bmp_fileheader	fh;
	t_bmp_infoheader	ih;
	t_bmp				res;
	int					*tmp;

	ft_memcpy(&fh, data, sizeof(t_bmp_fileheader));
	data += sizeof(t_bmp_fileheader);
	ft_memcpy(&ih, data, sizeof(t_bmp_infoheader));
	data += sizeof(t_bmp_infoheader);
	ft_bzero(&res, sizeof(t_bmp));
	if (bmp_error_check(fh, ih))
		ft_error("bmp memory read failed\n");
	res.image = (int *)malloc(sizeof(int) * ih.width * ih.height);
	tmp = (int *)malloc(sizeof(int) * ih.width * ih.height);
	if (!res.image || !tmp)
		ft_error("bmp memory allocation failed\n");
	res.width = ih.width;
	res.height = ih.height;
	ft_memcpy(tmp, data, sizeof(int) * ih.width * ih.height);
	bmp_set(&res, tmp);
	free(tmp);
	return (res);
}
