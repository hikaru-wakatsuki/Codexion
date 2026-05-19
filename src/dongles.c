/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 07:57:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/19 08:48:42 by hwakatsu         ###   ########.fr       */
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

static bool	push_requests(t_coder *coder, t_request *req, int first, int second)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->dongles[first].mutex);
	req->arrival_seq = sim->dongles[first].local_seq++;
	if (!push_request(&sim->dongles[first].wait_queue, *req, sim))
	{
		pthread_mutex_unlock(&sim->dongles[first].mutex);
		return (false);
	}
	pthread_mutex_unlock(&sim->dongles[first].mutex);
	pthread_mutex_lock(&sim->dongles[second].mutex);
	req->arrival_seq = sim->dongles[second].local_seq++;
	if (!push_request(&sim->dongles[second].wait_queue, *req, sim))
	{
		pthread_mutex_unlock(&sim->dongles[second].mutex);
		return (false);
	}
	pthread_mutex_unlock(&sim->dongles[second].mutex);
	return (true);
}

bool	take_dongles(t_coder *coder)
{
	t_request	req;
	int			first;
	int			second;
	t_sim		*sim;

	sim = coder->sim;
	set_order(&first, &second, coder);
	req = create_request(coder);
	if (!push_requests(coder, &req, first, second))
		return (false);
	while (!is_stopped(sim))
	{
		if (try_take_dongles(coder, &sim->dongles[first], &sim->dongles[second],
				&req))
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
		print_log(coder->sim, coder->id, "released a dongle");
		return ;
	}
	pthread_mutex_unlock(&dongle->mutex);
}
