/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_release.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 02:06:31 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/07 09:15:47 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner_coder_id == coder->id)
	{
		dongle->owner_coder_id = -1;
		dongle->cooldown_until_ms = get_time_ms() + coder->sim->dongle_cooldown;
		pthread_cond_broadcast(&dongle->cond);
		pthread_mutex_unlock(&dongle->mutex);
		print_log(coder->sim, coder->id, "released a dongle");
		return ;
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongles(t_coder *coder)
{
	release_dongle(coder, &coder->sim->dongles[coder->left_dongle_idx]);
	release_dongle(coder, &coder->sim->dongles[coder->right_dongle_idx]);
}
