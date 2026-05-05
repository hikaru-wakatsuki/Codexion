/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:52:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/05 06:07:26 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char *argv[])
{
	t_sim	*sim;
	int		i;

	if (!parse_args(&sim, argv))
		return (1);
	if (!init_sim(&sim))
		return (1);
	i = 0;
	while (i < sim->n_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
			return (1);
		i++;
	}
}

void	print_log(t_sim *sim, int id, char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d %s\n", timestamp_ms(sim), id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}
