/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 19:01:35 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/19 14:40:11 by hwakatsu         ###   ########.fr       */
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
	printf("%ld %d %s\n", get_time_ms() - sim->start_ms, id, msg);
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
