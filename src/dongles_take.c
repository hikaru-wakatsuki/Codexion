/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_take.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 07:57:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/15 15:32:17 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	set_order(int *first, int *second, t_coder *coder)
{
	if (coder->left_dongle_idx < coder->right_dongle_idx)
	{
		*first = coder->left_dongle_idx;
		*second = coder->right_dongle_idx;
	}
	else
	{
		*first = coder->right_dongle_idx;
		*second = coder->left_dongle_idx;
	}
}

bool	take_dongles(t_coder *coder)
{
	int	first;
	int	second;

	set_order(&first, &second, coder);
	while (!is_stopped(coder->sim))
	{
		if (!take_dongle(coder, &coder->sim->dongles[first]))
			return (false);
		if (!take_dongle(coder, &coder->sim->dongles[second]))
		{
			release_dongle(coder, &coder->sim->dongles[first]);
			return (false);
		}
		return (true);
	}
	return (false);
}

//static bool	is_my_turn(t_dongle *dongle, int coder_id)
//{
//	if (dongle->wait_queue.size == 0)
//		return (false);
//	return (dongle->wait_queue.data[0].coder_id == coder_id);
//}

//static void	push_both_requests(t_coder *coder, t_request req, int first,
//		int second)
//{
//	pthread_mutex_lock(&coder->sim->dongles[first].mutex);
//	pthread_mutex_lock(&coder->sim->dongles[second].mutex);
//	push_request(&coder->sim->dongles[first].wait_queue, req, coder->sim);
//	push_request(&coder->sim->dongles[second].wait_queue, req, coder->sim);
//	pthread_mutex_unlock(&coder->sim->dongles[second].mutex);
//	pthread_mutex_unlock(&coder->sim->dongles[first].mutex);
//}

//static void	remove_both_requests(t_coder *coder, int first, int second)
//{
//	pthread_mutex_lock(&coder->sim->dongles[first].mutex);
//	pthread_mutex_lock(&coder->sim->dongles[second].mutex);
//	remove_request(&coder->sim->dongles[first].wait_queue, coder->id,
//		coder->sim);
//	remove_request(&coder->sim->dongles[second].wait_queue, coder->id,
//		coder->sim);
//	pthread_mutex_unlock(&coder->sim->dongles[second].mutex);
//	pthread_mutex_unlock(&coder->sim->dongles[first].mutex);
//}

//static bool	try_take_both_dongles(t_coder *coder, int first, int second)
//{
//	t_sim		*sim;
//	t_dongle	*left;
//	t_dongle	*right;
//	long		now;

//	sim = coder->sim;
//	left = &sim->dongles[first];
//	right = &sim->dongles[second];
//	now = get_time_ms();
//	pthread_mutex_lock(&left->mutex);
//	pthread_mutex_lock(&right->mutex);
//	if (!is_my_turn(left, coder->id) || !is_my_turn(right, coder->id)
//		|| left->owner_coder_id != -1 || right->owner_coder_id != -1
//		|| now < left->cooldown_until_ms || now < right->cooldown_until_ms)
//	{
//		pthread_mutex_unlock(&right->mutex);
//		pthread_mutex_unlock(&left->mutex);
//		return (false);
//	}
//	pop_request(&left->wait_queue, sim);
//	pop_request(&right->wait_queue, sim);
//	left->owner_coder_id = coder->id;
//	right->owner_coder_id = coder->id;
//	pthread_mutex_unlock(&right->mutex);
//	pthread_mutex_unlock(&left->mutex);
//	print_log(coder->sim, coder->id, "has taken a dongle");
//	print_log(coder->sim, coder->id, "has taken a dongle");
//	return (true);
//}

//bool	take_dongles(t_coder *coder)
//{
//	t_request	req;
//	int			first;
//	int			second;

//	set_order(&first, &second, coder);
//	pthread_mutex_lock(&coder->sim->seq_mutex);
//	req.coder_id = coder->id;
//	req.arrival_seq = coder->sim->request_seq++;
//	pthread_mutex_lock(&coder->state_mutex);
//	req.deadline_ms = coder->last_compile_start_ms
//		+ coder->sim->time_to_burnout;
//	pthread_mutex_unlock(&coder->state_mutex);
//	pthread_mutex_unlock(&coder->sim->seq_mutex);
//	push_both_requests(coder, req, first, second);
//	while (!is_stopped(coder->sim))
//	{
//		if (try_take_both_dongles(coder, first, second))
//			return (true);
//		usleep(500);
//	}
//	remove_both_requests(coder, first, second);
//	return (false);
//}
