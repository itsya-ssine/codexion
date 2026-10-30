/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:44:30 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:44:32 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	print_usage(void)
{
	fprintf(stderr,
		"Usage: ./codexion num_coders time_to_burnout time_to_compile ");
	fprintf(stderr,
		"time_to_debug time_to_refactor num_compiles dongle_cooldown ");
	fprintf(stderr, "scheduler(fifo|edf)\n");
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (argc != 9)
	{
		print_usage();
		return (1);
	}
	if (parse_arguments(&sim, argc, argv))
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (init_simulation(&sim))
	{
		fprintf(stderr, "Error: while init resources\n");
		return (1);
	}
	if (launch_simulation(&sim) == 1)
		return (1);
	return (0);
}
