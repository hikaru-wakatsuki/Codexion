/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 20:19:40 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/05 05:54:03 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static bool	is_positive_number(char *str)
{
	if (!*str)
		return (false);
	if (str[0] == '0' && str[1] == '\0')
		return (false);
	while (*str)
	{
		if (!('0' <= *str && *str <= '9'))
			return (false);
		str++;
	}
	return (true);
}

bool	parse_args(t_sim *sim, char *argv[])
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!(is_positive_number(argv[i])))
			return (false);
		i++;
	}
	sim->n_coders = atoi(argv[1]);
	sim->time_to_burnout = (long)atoi(argv[2]);
	sim->time_to_compile = (long)atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->must_compile_count = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (false);
	return (true);
}
