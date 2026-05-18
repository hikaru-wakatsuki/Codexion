/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_take.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 07:57:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/18 16:39:23 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	set_order(int *first, int *second, t_coder *coder)
{
	if (coder->left_dongle_idx < coder->right_dongle_idx)
	{
		*first = coder->left_dongle_idx;
		*second = coder->right_dongle_idx;
	}
	else
	{
		*first = coder->right_dongle_idx;
		*second = coder->left_dongle_idx;
	}
}

static bool	no_higher_priority(t_dongle *dongle, t_request *req, t_sim *sim)
{
	int	i;

	i = 0;
	while (i < dongle->wait_queue.size)
	{
		if (dongle->wait_queue.data[i].coder_id == req->coder_id)
		{
			i++;
			continue ;
		}
		if (is_higher_priority(dongle->wait_queue.data[i], *req, sim))
			return (false);
		i++;
	}
	return (true);
}

bool	is_higher_priority(t_request a, t_request b, t_sim *sim)
{
	if (sim->scheduler == FIFO)
		return (a.arrival_seq < b.arrival_seq);
	if (a.deadline_ms != b.deadline_ms)
		return (a.deadline_ms < b.deadline_ms);
	return (a.arrival_seq < b.arrival_seq);
}

static void	push_both_requests(t_coder *coder, t_request *req, int first,
		int second)
{
	pthread_mutex_lock(&coder->sim->dongles[first].mutex);
	req->arrival_seq = coder->sim->dongles[first].local_seq++;
	push_request(&coder->sim->dongles[first].wait_queue, *req, coder->sim);
	pthread_mutex_unlock(&coder->sim->dongles[first].mutex);
	pthread_mutex_lock(&coder->sim->dongles[second].mutex);
	req->arrival_seq = coder->sim->dongles[second].local_seq++;
	push_request(&coder->sim->dongles[second].wait_queue, *req, coder->sim);
	pthread_mutex_unlock(&coder->sim->dongles[second].mutex);
}

static bool	try_take_both_dongles(t_coder *coder, int first, int second,
		t_request *req)
{
	t_sim		*sim;
	t_dongle	*first_take_dongle;
	t_dongle	*second_take_dongle;
	long		now;

	sim = coder->sim;
	first_take_dongle = &sim->dongles[first];
	second_take_dongle = &sim->dongles[second];
	now = get_time_ms();
	pthread_mutex_lock(&first_take_dongle->mutex);
	pthread_mutex_lock(&second_take_dongle->mutex);
	if (!no_higher_priority(first_take_dongle, req, sim)
		|| !no_higher_priority(second_take_dongle, req, sim)
		|| first_take_dongle->owner_coder_id != -1 || second_take_dongle->owner_coder_id != -1
		|| now < first_take_dongle->cooldown_until_ms || now < second_take_dongle->cooldown_until_ms)
	{
		pthread_mutex_unlock(&second_take_dongle->mutex);
		pthread_mutex_unlock(&first_take_dongle->mutex);
		return (false);
	}
	pop_request(&first_take_dongle->wait_queue, sim);
	pop_request(&second_take_dongle->wait_queue, sim);
	first_take_dongle->owner_coder_id = coder->id;
	second_take_dongle->owner_coder_id = coder->id;
	pthread_mutex_unlock(&second_take_dongle->mutex);
	pthread_mutex_unlock(&first_take_dongle->mutex);
	print_log(coder->sim, coder->id, "has taken a dongle");
	print_log(coder->sim, coder->id, "has taken a dongle");
	return (true);
}

bool	take_dongles(t_coder *coder)
{
	t_request	req;
	int			first;
	int			second;

	set_order(&first, &second, coder);
	req.coder_id = coder->id;
	pthread_mutex_lock(&coder->state_mutex);
	req.deadline_ms = coder->last_compile_start_ms
		+ coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	push_both_requests(coder, &req, first, second);
	while (!is_stopped(coder->sim))
	{
		if (try_take_both_dongles(coder, first, second, &req))
			return (true);
		usleep(500);
	}
	return (false);
}
