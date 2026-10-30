/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_operations.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:45:23 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:45:24 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_push(t_heap *h, long long priority, int compile_count,
		int coder_id)
{
	t_node	new_node;

	if (!h || h->size == h->capacity)
		return (1);
	new_node.coder_id = coder_id;
	new_node.priority = priority;
	new_node.compile_count = compile_count;
	h->data[h->size] = new_node;
	h->size++;
	sift_up(h, h->size - 1);
	return (0);
}

t_node	heap_pop(t_heap *h)
{
	t_node	empty;
	t_node	data;

	if (!h || !h->size)
	{
		empty.priority = -1;
		empty.compile_count = -1;
		empty.coder_id = -1;
		return (empty);
	}
	data = h->data[0];
	h->data[0] = h->data[h->size - 1];
	h->size--;
	sift_down(h, 0);
	return (data);
}

int	heap_peek_id(t_heap *h)
{
	if (!h || !h->data || !h->size)
		return (-1);
	return (h->data[0].coder_id);
}
