/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:01:00 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/18 16:13:54 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	push_request(t_heap *heap, t_request req, t_sim *sim)
{
	if (heap->size == heap->capacity)
		return ;
	heap->data[heap->size] = req;
	heap->size++;
	shift_up(heap, heap->size - 1, sim);
}

t_request	pop_request(t_heap *heap, t_sim *sim)
{
	t_request	top;

	if (heap->size == 0)
		return ((t_request){0});
	top = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	heap->size--;
	shift_down(heap, 0, sim);
	return (top);
}

bool	remove_request(t_heap *heap, int coder_id, t_sim *sim)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->data[i].coder_id == coder_id)
		{
			heap->data[i] = heap->data[heap->size - 1];
			heap->size--;
			if (i < heap->size)
			{
				shift_up(heap, i, sim);
				shift_down(heap, i, sim);
			}
			return (true);
		}
		i++;
	}
	return (false);
}
