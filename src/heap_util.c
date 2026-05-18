/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_util.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:01:00 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/18 16:15:22 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

bool	is_higher_priority(t_request a, t_request b, t_sim *sim)
{
	if (sim->scheduler == FIFO)
		return (a.arrival_seq < b.arrival_seq);
	if (a.deadline_ms != b.deadline_ms)
		return (a.deadline_ms < b.deadline_ms);
	return (a.arrival_seq < b.arrival_seq);
}

static void	swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	shift_up(t_heap *heap, int idx, t_sim *sim)
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

void	shift_down(t_heap *heap, int idx, t_sim *sim)
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
