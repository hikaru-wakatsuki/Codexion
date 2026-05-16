/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:52:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/16 14:53:20 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	join_pthreads(t_sim *sim)
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

bool	start_pthread(t_sim *sim)
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

int	main(int argc, char *argv[])
{
	t_sim	sim;
	bool	success;

	if (!parse_args(argc, argv, &sim))
		return (1);
	if (!init_sim(&sim))
		return (1);
	success = start_pthread(&sim);
	join_pthreads(&sim);
	cleanup_sim(&sim);
	if (success)
		return (0);
	return (1);
}
