/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:01:00 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/18 16:39:57 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	shift_up(t_heap *heap, int idx, t_sim *sim)
{
	int	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (!is_higher_priority(heap->data[idx], heap->data[parent], sim))
			break ;
		swap(&heap->data[idx], &heap->data[parent]);
		idx = parent;
	}
}

static void	shift_down(t_heap *heap, int idx, t_sim *sim)
{
	int			left;
	int			right;
	int			highest_idx;

	while (true)
	{
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		highest_idx = idx;
		if (left < heap->size && is_higher_priority(heap->data[left],
				heap->data[highest_idx], sim))
			highest_idx = left;
		if (right < heap->size && is_higher_priority(heap->data[right],
				heap->data[highest_idx], sim))
			highest_idx = right;
		if (highest_idx == idx)
			break ;
		swap(&heap->data[idx], &heap->data[highest_idx]);
		idx = highest_idx;
	}
}

bool	push_request(t_heap *heap, t_request req, t_sim *sim)
{
	if (heap->size == heap->capacity)
		return (false);
	heap->data[heap->size] = req;
	heap->size++;
	shift_up(heap, heap->size - 1, sim);
	return (true);
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
