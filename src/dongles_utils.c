/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 02:06:31 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/19 08:48:56 by hwakatsu         ###   ########.fr       */
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

static bool	can_take(t_dongle *dongle, t_request *req, long now, t_sim *sim)
{
	return (no_higher_priority(dongle, req, sim) && dongle->owner_coder_id == -1
		&& now >= dongle->cooldown_until_ms);
}

bool	try_take_dongles(t_coder *coder, t_dongle *first_d,
		t_dongle *second_d, t_request *req)
{
	t_sim	*sim;
	long	now;

	sim = coder->sim;
	now = get_time_ms();
	pthread_mutex_lock(&first_d->mutex);
	pthread_mutex_lock(&second_d->mutex);
	if (can_take(first_d, req, now, sim) && can_take(second_d, req, now, sim))
	{
		pop_request(&first_d->wait_queue, sim);
		pop_request(&second_d->wait_queue, sim);
		first_d->owner_coder_id = coder->id;
		second_d->owner_coder_id = coder->id;
		pthread_mutex_unlock(&second_d->mutex);
		pthread_mutex_unlock(&first_d->mutex);
		print_log(sim, coder->id, "has taken a dongle");
		print_log(sim, coder->id, "has taken a dongle");
		return (true);
	}
	pthread_mutex_unlock(&second_d->mutex);
	pthread_mutex_unlock(&first_d->mutex);
	return (false);
}
