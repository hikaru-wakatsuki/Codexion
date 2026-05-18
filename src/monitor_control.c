/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_control.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 20:19:40 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/18 13:51:46 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	broadcast_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

static void	handle_burnout(t_sim *sim, int id)
{
	pthread_mutex_lock(&sim->stop_mutex);
	if (!sim->stop_simulation)
	{
		sim->stop_simulation = true;
		pthread_mutex_unlock(&sim->stop_mutex);
		broadcast_dongles(sim);
		print_log(sim, id, "burned out");
	}
	else
		pthread_mutex_unlock(&sim->stop_mutex);
}

static bool	check_burnout(t_sim *sim)
{
	int		i;
	long	now;

	i = 0;
	now = get_time_ms();
	while (i < sim->n_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		if (now - sim->coders[i].last_compile_start_ms >= sim->time_to_burnout)
		{
			pthread_mutex_unlock(&sim->coders[i].state_mutex);
			handle_burnout(sim, i + 1);
			return (true);
		}
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		i++;
	}
	return (false);
}

static bool	check_all_finished(t_sim *sim)
{
	bool	all_finished;
	bool	should_stop;

	if (sim->must_compile_count <= 0)
		return (false);
	pthread_mutex_lock(&sim->finish_mutex);
	all_finished = (sim->finished_count == sim->n_coders);
	pthread_mutex_unlock(&sim->finish_mutex);
	if (all_finished)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		should_stop = !sim->stop_simulation;
		if (should_stop)
			sim->stop_simulation = true;
		pthread_mutex_unlock(&sim->stop_mutex);
		if (should_stop)
			broadcast_dongles(sim);
		return (true);
	}
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_stopped(sim))
	{
		if (check_all_finished(sim))
			break ;
		if (check_burnout(sim))
			break ;
		smart_sleep(1, sim);
	}
	return (NULL);
}
