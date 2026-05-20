/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 19:01:35 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/20 01:03:57 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	print_log(t_sim *sim, int id, char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	//printf("%ld %d %s\n", get_time_ms() - sim->start_ms, id, msg);
	long now = get_time_ms();
	printf("%ld %d %s %ld\n",now - sim->start_ms, id, msg, now - sim->coders[id - 1].last_compile_start_ms);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	smart_sleep(long ms, t_sim *sim)
{
	long	start;

	start = get_time_ms();
	while (!is_stopped(sim))
	{
		if (get_time_ms() - start >= ms)
			break ;
		usleep(100);
	}
}
