/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:01:00 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/04 11:18:38 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static bool	is_a_higher_priority(t_request a, t_request b, t_sim *sim)
{
	if (sim->scheduler == FIFO)
		return (a.arrival_seq < b.arrival_seq);
	else
		return (a.deadline_ms < b.deadline_ms);
}

static void	swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	push_request(t_heap *heap, t_request req, t_sim *sim)
{
	int	i;
	int	parent;

	if (heap->size == heap->capacity)
		return ;
	i = heap->size;
	heap->data[i] = req;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!is_a_higher_priority(heap->data[i], heap->data[parent], sim))
			break ;
		swap(&heap->data[i], &heap->data[parent]);
		i = parent;
	}
}

static void	shift_down(t_heap *heap, int i, t_sim *sim)
{
	int			left;
	int			right;
	int			smallest;

	while (true)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size && is_a_higher_priority(heap->data[left],
				heap->data[smallest], sim))
			smallest = left;
		if (right < heap->size && is_a_higher_priority(heap->data[right],
				heap->data[smallest], sim))
			smallest = right;
		if (smallest == i)
			break ;
		swap(&heap->data[i], &heap->data[smallest]);
		i = smallest;
	}
}

t_request	pop_request(t_heap *heap, t_sim *sim)
{
	t_request	top;
	int			i;

	if (heap->size == 0)
		return ((t_request){0});
	top = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	heap->size--;
	shift_down(heap, 0, sim);
	return (top);
}
