/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/29 18:42:16 by vkuikka           #+#    #+#             */
/*   Updated: 2019/10/29 18:42:19 by vkuikka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, t_list *(*f)(t_list *elem))
{
	t_list	*start;
	t_list	*new;

	new = ft_lstnew(f(lst)->content, f(lst)->content_size);
	if (!new)
		return (NULL);
	start = new;
	while (lst->next)
	{
		lst = lst->next;
		new->next = ft_lstnew(f(lst)->content, f(lst)->content_size);
		if (!new->next)
			return (NULL);
		new = new->next;
	}
	return (start);
}
