/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 12:47:07 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/04 12:21:34 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static bool	is_my_turn(t_dongle *dongle, int coder_id)
{
	t_request	top;

	if (dongle->wait_queue.size == 0)
		return (false);
	top = dongle->wait_queue.data[0];
	return (top.coder_id == coder_id);
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
	while (!is_my_turn(dongle, coder->id)
		|| get_time_ms() < dongle->cooldown_until_ms)
	{
		if (is_stopped(coder->sim))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
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

static void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner_coder_id = -1;
	dongle->cooldown_until_ms = get_time_ms() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
	print_log(coder->sim, coder->id, "released a dongle");
}

void	release_dongles(t_coder *coder)
{
	release_dongle(coder, &coder->sim->dongles[coder->left_dongle_idx]);
	release_dongle(coder, &coder->sim->dongles[coder->right_dongle_idx]);
}
