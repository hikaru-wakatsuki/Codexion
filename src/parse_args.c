/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 20:19:40 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/23 16:46:55 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static bool	is_non_negative_number(char *str)
{
	size_t	len;
	int		i;

	if (!str || !*str)
		return (false);
	i = 0;
	while (str[i])
	{
		if (!('0' <= str[i] && str[i] <= '9'))
			return (false);
		i++;
	}
	while (*str == '0' && *(str + 1) != '\0')
		str++;
	len = strlen(str);
	if (len > 10)
		return (false);
	else if (len == 10 && strcmp(str, "2147483647") > 0)
		return (false);
	return (true);
}

static bool	validate_args(char *argv[])
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!is_non_negative_number(argv[i]))
			return (false);
		i++;
	}
	return (true);
}

static bool	set_scheduler(char *str, t_sim *sim)
{
	if (strcmp(str, "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(str, "edf") == 0)
		sim->scheduler = EDF;
	else
		return (false);
	return (true);
}

bool	parse_args(int argc, char *argv[], t_sim *sim)
{
	if (argc != 9)
		return (false);
	if (!validate_args(argv))
		return (false);
	sim->n_coders = atoi(argv[1]);
	if (sim->n_coders == 0 || sim->n_coders > MAX_CODERS)
		return (false);
	sim->time_to_burnout = (long)atoi(argv[2]);
	sim->time_to_compile = (long)atoi(argv[3]);
	sim->time_to_debug = (long)atoi(argv[4]);
	sim->time_to_refactor = (long)atoi(argv[5]);
	sim->must_compile_count = atoi(argv[6]);
	sim->dongle_cooldown = (long)atoi(argv[7]);
	if (!set_scheduler(argv[8], sim))
		return (false);
	return (true);
}
