/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 19:01:35 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/04/27 23:08:04 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

long	timestamp_ms(t_sim *sim)
{
	return (get_time_ms() - sim->start_ms);
}

bool	is_stopped(t_sim *sim)
{
	bool	ret;

	pthread_mutex_lock(&sim->stop_mutex);
	ret = sim->stop_simulation;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (ret);
}

void	smart_sleep(long ms, t_sim *sim)
{
	long	start;

	start = get_time_ms();
	while (!is_stopped(sim))
	{
		if (get_time_ms() - start >= ms)
			break ;
		usleep(500);
	}
}
