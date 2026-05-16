/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 19:52:49 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/16 16:28:23 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char *argv[])
{
	t_sim	sim;
	bool	success;

	if (!parse_args(argc, argv, &sim))
		return (1);
	if (!init_sim(&sim))
		return (1);
	success = start_threads(&sim);
	join_threads(&sim);
	cleanup_sim(&sim);
	if (success)
		return (0);
	return (1);
}
