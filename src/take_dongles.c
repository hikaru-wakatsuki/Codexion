/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 12:47:07 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/04 10:01:26 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	take_dongles(t_coder *coder)
{
	int	first;
	int	second;

	if (coder->left_dongle_idx < coder->right_dongle_idx)
	{
		first = coder->left_dongle_idx;
		second = coder->right_dongle_idx;
	}
	else
	{
		first = coder->right_dongle_idx;
		second = coder->left_dongle_idx;
	}
	pthread_mutex_lock(&coder->sim->dongles[first].mutex);
	print_log(coder->sim, coder->id, "has taken a dongle");
	pthread_mutex_lock(&coder->sim->dongles[second].mutex);
	print_log(coder->sim, coder->id, "has taken a dongle");
}

void	release_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->sim->dongles[coder->left_dongle_idx].mutex);
	pthread_mutex_unlock(&coder->sim->dongles[coder->right_dongle_idx].mutex);
}
