/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 20:19:40 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/05 01:42:07 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

bool	check_burnout(t_sim *sim)
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
			pthread_mutex_lock(&sim->log_mutex);
			printf("%ld %d burned out\n", timestamp_ms(sim), i + 1);
			pthread_mutex_unlock(&sim->log_mutex);
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop_simulation = true;
			pthread_mutex_unlock(&sim->stop_mutex);
			return (true);
		}
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		i++;
	}
	return (false);
}

bool	check_all_finish(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n_coders)
	{
		if 
		i++;
	}
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop_simulation = true;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (true);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_stopped(sim))
	{
		if (check_burnout(sim))
			break ;
		if (check_all_finish(sim))
			break ;
		smart_sleep(1, sim);
	}
	return (NULL);
}
