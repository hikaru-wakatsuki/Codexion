/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_take_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 07:57:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/09 09:03:18 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	set_order(int *first, int *second, t_coder *coder)
{
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
}

static long	get_deadline(t_coder *coder)
{
	long	deadline;

	pthread_mutex_lock(&coder->state_mutex);
	deadline = coder->last_compile_start_ms + coder->sim->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	return (deadline);
}

static bool	should_take_first_now(t_coder *coder, int first)
{
	t_coder	*left;
	t_coder	*right;
	t_sim	*sim;

	sim = coder->sim;
	if (sim->scheduler != EDF)
		return (true);
	left = &sim->coders[(coder->id - 2 + sim->n_coders) % sim->n_coders];
	right = &sim->coders[coder->id % sim->n_coders];
	if (get_deadline(left) < get_deadline(coder))
		return (false);
	if (get_deadline(right) < get_deadline(coder))
		return (false);
	return (true);
}

bool	take_dongles(t_coder *coder)
{
	int	first;
	int	second;

	set_order(&first, &second, coder);
	while (!is_stopped(coder->sim))
	{
		if (!should_take_first_now(coder, first))
		{
			usleep(500);
			continue ;
		}
	}
	if (!take_dongle(coder, &coder->sim->dongles[first]))
		return (false);
	if (!take_dongle(coder, &coder->sim->dongles[second]))
	{
		release_dongle(coder, &coder->sim->dongles[first]);
		return (false);
	}
	return (true);
}
