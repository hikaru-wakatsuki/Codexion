/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 12:26:38 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/07 09:21:04 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	do_compile(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start_ms = get_time_ms();
	pthread_mutex_unlock(&coder->state_mutex);
	print_log(sim, coder->id, "is compiling");
	smart_sleep(sim->time_to_compile, sim);
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->state_mutex);
}

bool	is_finished(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&coder->state_mutex);
	if (coder->compile_count >= sim->must_compile_count)
	{
		pthread_mutex_unlock(&coder->state_mutex);
		return (true);
	}
	pthread_mutex_unlock(&coder->state_mutex);
	return (false);
}

static bool	check_finish(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	if (sim->must_compile_count > 0 && is_finished(coder))
	{
		pthread_mutex_lock(&sim->finish_mutex);
		sim->finished_count++;
		pthread_mutex_unlock(&sim->finish_mutex);
		return (true);
	}
	return (false);
}

static void	do_rest(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	print_log(sim, coder->id, "is debugging");
	smart_sleep(sim->time_to_debug, sim);
	print_log(sim, coder->id, "is refactoring");
	smart_sleep(sim->time_to_refactor, sim);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (!is_stopped(sim))
	{
		if (!take_dongles(coder))
			break ;
		do_compile(coder);
		release_dongles(coder);
		if (check_finish(coder))
			break ;
		do_rest(coder);
	}
	return (NULL);
}
