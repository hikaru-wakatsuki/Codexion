/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 21:35:30 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/04/27 23:00:36 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

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
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->finish_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->seq_mutex);
	free(sim->coders);
	free(sim->dongles);
}
