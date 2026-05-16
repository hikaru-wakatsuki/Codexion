/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_control.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 16:19:29 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/16 16:26:00 by hwakatsu         ###   ########.fr       */
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
			return (false);
		sim->coders[i].is_thread_created = true;
		i++;
	}
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
		return (false);
	sim->is_thread_created = true;
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
	if (sim->is_thread_created)
		pthread_join(sim->monitor_thread, NULL);
}
