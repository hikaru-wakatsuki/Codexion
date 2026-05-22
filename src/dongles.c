/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 07:57:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/22 16:20:51 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	set_order(t_dongle **first, t_dongle **second, t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	if (coder->left_dongle_idx < coder->right_dongle_idx)
	{
		*first = &sim->dongles[coder->left_dongle_idx];
		*second = &sim->dongles[coder->right_dongle_idx];
	}
	else
	{
		*first = &sim->dongles[coder->right_dongle_idx];
		*second = &sim->dongles[coder->left_dongle_idx];
	}
}

static t_request	create_request(t_coder *coder)
{
	t_request	req;

	req.coder_id = coder->id;
	pthread_mutex_lock(&coder->state_mutex);
	req.deadline_ms = coder->last_compile_start_ms
		+ coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	req.arrival_seq = 0;
	return (req);
}

static bool	push_requests(t_coder *coder, t_req_pair *reqs, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->local_seq == 0)
	{
		if (coder->sim->n_coders % 2 == 1 && dongle->id == 0)
			reqs->first.arrival_seq = dongle->local_seq;
		else
			reqs->first.arrival_seq = (coder->id % 2 == 1) ? 0 : 1;
		dongle->local_seq = 1;
	}
	else if (dongle->local_seq == 1)
	{
		if (coder->sim->n_coders % 2 == 1 && dongle->id == 0)
			reqs->first.arrival_seq = dongle->local_seq;
		else
			reqs->first.arrival_seq = (coder->id % 2 == 1) ? 0 : 1;
		dongle->local_seq = 2;
	}
	else
		reqs->first.arrival_seq = dongle->local_seq++;
	if (!push_request(&dongle->wait_queue, reqs->first, coder->sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (false);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (true);
}

bool	take_dongles(t_coder *coder)
{
	t_req_pair	reqs;
	t_dongle	*first;
	t_dongle	*second;

	set_order(&first, &second, coder);
	reqs.first = create_request(coder);
	reqs.second = create_request(coder);
	if (!push_requests(coder, &reqs, first))
		return (false);
	if (!push_requests(coder, &reqs, second))
		return (false);
	while (!is_stopped(coder->sim))
	{
		if (try_take_dongles(coder, first, second, &reqs))
			return (true);
		usleep(500);
	}
	return (false);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner_coder_id == coder->id)
	{
		dongle->owner_coder_id = -1;
		dongle->cooldown_until_ms = get_time_ms() + coder->sim->dongle_cooldown;
		pthread_mutex_unlock(&dongle->mutex);
		return ;
	}
	pthread_mutex_unlock(&dongle->mutex);
}
