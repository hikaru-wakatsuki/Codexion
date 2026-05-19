/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_control.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 21:09:06 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/19 08:29:56 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static bool	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = (t_coder *)malloc(sizeof(t_coder) * sim->n_coders);
	if (!sim->coders)
		return (false);
	i = 0;
	while (i < sim->n_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].is_thread_created = false;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start_ms = sim->start_ms;
		sim->coders[i].sim = sim;
		sim->coders[i].left_dongle_idx = i;
		sim->coders[i].right_dongle_idx = (i + 1) % sim->n_coders;
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL))
			return (false);
		i++;
	}
	return (true);
}

static bool	init_dongles(t_sim *sim)
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
		sim->dongles[i].local_seq = 0;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL))
			return (false);
		sim->dongles[i].wait_queue.size = 0;
		sim->dongles[i].wait_queue.capacity = sim->n_coders;
		sim->dongles[i].wait_queue.data
			= (t_request *)malloc(sizeof(t_request) * sim->n_coders);
		if (!sim->dongles[i].wait_queue.data)
			return (false);
		i++;
	}
	return (true);
}

bool	init_sim(t_sim *sim)
{
	sim->start_ms = get_time_ms();
	sim->stop_simulation = false;
	sim->finished_count = 0;
	sim->is_monitor_created = false;
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&sim->finish_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&sim->log_mutex, NULL))
		return (false);
	if (!init_coders(sim))
		return (false);
	if (!init_dongles(sim))
		return (false);
	return (true);
}

bool	is_stopped(t_sim *sim)
{
	bool	ret;

	pthread_mutex_lock(&sim->stop_mutex);
	ret = sim->stop_simulation;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (ret);
}

void	cleanup_sim(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n_coders)
		pthread_mutex_destroy(&sim->coders[i++].state_mutex);
	i = 0;
	while (i < sim->n_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		free(sim->dongles[i].wait_queue.data);
		i++;
	}
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->finish_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	free(sim->coders);
	free(sim->dongles);
}
