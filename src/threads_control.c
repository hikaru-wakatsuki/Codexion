/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_control.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 16:19:29 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/20 13:25:08 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

bool	start_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop_simulation = true;
			pthread_mutex_unlock(&sim->stop_mutex);
			return (false);
		}
		sim->coders[i].is_thread_created = true;
		i++;
	}
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		sim->stop_simulation = true;
		pthread_mutex_unlock(&sim->stop_mutex);
		return (false);
	}
	sim->is_monitor_created = true;
	return (true);
}

void	join_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n_coders)
	{
		if (sim->coders[i].is_thread_created)
			pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	if (sim->is_monitor_created)
		pthread_join(sim->monitor_thread, NULL);
}
