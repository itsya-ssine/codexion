/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:43:30 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:43:33 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	fill_params(t_sim *sim, char **argv)
{
	sim->params.number_of_coders = atoi(argv[1]);
	sim->params.time_to_burnout = atoi(argv[2]);
	sim->params.time_to_compile = atoi(argv[3]);
	sim->params.time_to_debug = atoi(argv[4]);
	sim->params.time_to_refactor = atoi(argv[5]);
	sim->params.number_of_compiles_required = atoi(argv[6]);
	sim->params.dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "edf") == 0)
		sim->params.scheduler = SCHEDULER_EDF;
	else
		sim->params.scheduler = SCHEDULER_FIFO;
}

int	parse_arguments(t_sim *sim, int argc, char **argv)
{
	int	i;

	if (!argv || !*argv || argc != 9)
		return (1);
	i = 1;
	while (i < 8)
	{
		if (!is_valid_number(argv[i]))
			return (1);
		i++;
	}
	if (strcmp(argv[8], "edf") != 0 && strcmp(argv[8], "fifo") != 0)
	{
		fprintf(stderr, "Error: scheduler must be 'fifo' or 'edf'\n");
		return (1);
	}
	fill_params(sim, argv);
	if (sim->params.number_of_coders == 0)
		return (1);
	return (0);
}
