/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_take.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 07:57:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/14 09:11:41 by hwakatsu         ###   ########.fr       */
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
