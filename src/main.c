/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:52:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/05 12:53:08 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	shutdown_sim(t_sim *sim, int created, bool monitor_created)
{
	int	i;

	i = 0;
	while (i < created)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	if (monitor_created)
		pthread_join(sim->monitor_thread, NULL);
	cleanup_sim(sim);
}

int	main(int argc, char *argv[])
{
	t_sim	sim;
	int		i;
	bool	monitor_created;

	monitor_created = false;
	if (!parse_args(argc, argv, &sim))
		return (1);
	if (!init_sim(&sim))
		return (1);
	i = 0;
	while (i < sim.n_coders)
	{
		if (pthread_create(&sim.coders[i].thread, NULL,
				coder_routine, &sim.coders[i]) != 0)
			return (shutdown_sim(&sim, i, monitor_created), 1);
		i++;
	}
	if (pthread_create(&sim.monitor_thread, NULL, monitor_routine, &sim) != 0)
		return (shutdown_sim(&sim, sim.n_coders, monitor_created), 1);
	else
		monitor_created = true;
	return (shutdown_sim(&sim, sim.n_coders, monitor_created), 0);
}
