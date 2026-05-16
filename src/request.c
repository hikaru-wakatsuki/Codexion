/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:01:00 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/15 16:39:01 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

bool	is_a_higher_priority(t_request a, t_request b, t_sim *sim)
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

static void	shift_up(t_heap *heap, int i, t_sim *sim)
{
	int	parent;

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
				shift_up(heap, i , sim);
				shift_down(heap, i, sim);
			}
			return (true);
		}
		i++;
	}
	return (false);
}
