/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 02:06:31 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/21 15:13:08 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

bool	is_higher_priority(t_request a, t_request b, t_sim *sim)
{
	if (sim->scheduler == EDF && a.deadline_ms != b.deadline_ms)
		return (a.deadline_ms < b.deadline_ms);
	if (a.arrival_seq != b.arrival_seq)
		return (a.arrival_seq < b.arrival_seq);
	return (a.coder_id < b.coder_id);
}

//static bool	conflicts_with(int coder_id, t_dongle *dongle, int n_coders)
//{
//    int left = (coder_id - 1 + n_coders) % n_coders;
//    int right = coder_id % n_coders;
//    return (dongle->id == left || dongle->id == right);
//}

static bool	no_higher_priority(t_dongle *dongle, t_dongle *neighbor,
		t_request *req, t_sim *sim)
{
	int	i;
	int	j;

	i = 0;
	while (i < dongle->wait_queue.size)
	{
		if (dongle->wait_queue.data[i].coder_id != req->coder_id
			&& is_higher_priority(dongle->wait_queue.data[i], *req, sim))
		{
			j = 0;
			while (j < neighbor->wait_queue.size)
			{
				if (neighbor->wait_queue.data[j].coder_id
					== dongle->wait_queue.data[i].coder_id)
					return (false);
				j++;
			}
		}
		i++;
	}
	return (true);
}

static bool	can_take(t_dongle *first, t_dongle *second, t_req_pair *reqs,
	t_sim *sim)
{
	return (no_higher_priority(first, second, &reqs->first, sim)
		&& no_higher_priority(second, first, &reqs->second, sim)
		&& first->owner_coder_id == -1
		&& second->owner_coder_id == -1);
}

bool	try_take_dongles(t_coder *coder, t_dongle *first, t_dongle *second,
		t_req_pair *reqs)
{
	t_sim	*sim;
	long	now;

	sim = coder->sim;
	now = get_time_ms();
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	if (can_take(first, second, reqs, sim)
		&& now >= first->cooldown_until_ms
		&& now >= second->cooldown_until_ms)
	{
		remove_request(&first->wait_queue, reqs->first, sim);
		remove_request(&second->wait_queue, reqs->second, sim);
		first->owner_coder_id = coder->id;
		second->owner_coder_id = coder->id;
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
		print_log(sim, coder->id, "has taken a dongle");
		print_log(sim, coder->id, "has taken a dongle");
		return (true);
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (false);
}
