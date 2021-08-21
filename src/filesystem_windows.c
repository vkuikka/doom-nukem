/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filesystem_windows.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpehkone <rpehkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/21 04:01:26 by rpehkone          #+#    #+#             */
/*   Updated: 2021/08/21 04:46:50 by rpehkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom_nukem.h"

char	*get_current_directory(void)
{
	TCHAR	szFileName[PATH_MAX];
	char	*str;

	str = (char *)malloc(sizeof(char) * PATH_MAX);
	if (!str)
		ft_error("memory allocation failed\n");
	GetModuleFileName(NULL, szFileName, PATH_MAX);
	ft_strcpy(str, szFileName);
	path_up_dir(str);
	return (str);
}
