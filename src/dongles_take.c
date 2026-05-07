/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_take.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 12:47:07 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/07 02:39:19 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static bool	can_take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	top;

	if (dongle->wait_queue.size == 0)
		return (false);
	top = dongle->wait_queue.data[0];
	return (top.coder_id == coder->id
		&& dongle->owner_coder_id == -1
		&& get_time_ms() >= dongle->cooldown_until_ms);
}

static void	ms_to_timespec(long ms, struct timespec *ts)
{
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1000000;
}

static bool	wait_for_turn(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;

	while (!can_take_dongle(coder, dongle))
	{
		if (is_stopped(coder->sim))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return (false);
		}
		if (dongle->wait_queue.size > 0
			&& dongle->wait_queue.data[0].coder_id == coder->id
			&& dongle->owner_coder_id == -1)
		{
			ms_to_timespec(dongle->cooldown_until_ms, &ts);
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	return (true);
}

static void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

	pthread_mutex_lock(&coder->sim->seq_mutex);
	req.coder_id = coder->id;
	req.arrival_seq = coder->sim->request_seq++;
	req.deadline_ms = get_time_ms() + coder->sim->time_to_compile;
	pthread_mutex_unlock(&coder->sim->seq_mutex);
	pthread_mutex_lock(&dongle->mutex);
	push_request(&dongle->wait_queue, req, coder->sim);
	if (!wait_for_turn(coder, dongle))
		return ;
	pop_request(&dongle->wait_queue, coder->sim);
	dongle->owner_coder_id = coder->id;
	pthread_mutex_unlock(&dongle->mutex);
	print_log(coder->sim, coder->id, "has taken a dongle");
}

void	take_dongles(t_coder *coder)
{
	int	first;
	int	second;

	if (coder->left_dongle_idx < coder->right_dongle_idx)
	{
		first = coder->left_dongle_idx;
		second = coder->right_dongle_idx;
	}
	else
	{
		first = coder->right_dongle_idx;
		second = coder->left_dongle_idx;
	}
	take_dongle(coder, &coder->sim->dongles[first]);
	take_dongle(coder, &coder->sim->dongles[second]);
}
