/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_sim.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 21:09:06 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/04/27 22:38:28 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

bool	init_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&sim->finish_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&sim->log_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&sim->seq_mutex, NULL))
		return (false);
	return (true);
}

bool	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = (t_coder *)malloc(sizeof(t_coder) * sim->n_coders);
	if (!sim->coders)
		return (false);
	i = 0;
	while (i < sim->n_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start_ms = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left_dongle_idx = i;
		sim->coders[i].right_dongle_idx = (i + 1) % sim->n_coders;
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL))
			return (false);
		i++;
	}
	return (true);
}

bool	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = (t_dongle *)malloc(sizeof(t_dongle) * sim->n_coders);
	if (!sim->dongles)
		return (false);
	i = 0;
	while (i < sim->n_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].owner_coder_id = -1;
		sim->dongles[i].cooldown_until_ms = 0;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL))
			return (false);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL))
			return (false);
		sim->dongles[i].wait_queue.size = 0;
		sim->dongles[i].wait_queue.capacity = 0;
		sim->dongles[i].wait_queue.data = NULL;
		i++;
	}
	return (true);
}

bool	init_monitor(t_sim *sim)
{
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim))
		return (false);
	return (true);
}

bool	init_sim(t_sim *sim)
{

	if (!init_mutexes(sim))
		return (false);
	if (!init_coders(sim))
		return (false);
	if (!init_dongles(sim))
		return (false);
	if (!init_monitor(sim))
		return (false);
	return (true);
}
