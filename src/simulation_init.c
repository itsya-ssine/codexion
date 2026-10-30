/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:44:06 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:44:07 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coders(t_sim *sim)
{
	int			i;
	long long	current_time;

	i = -1;
	current_time = get_time_ms();
	while (++i < sim->params.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		if (pthread_mutex_init(&sim->coders[i].coder_mutex, NULL))
		{
			cleanup_partial_init(sim, i, 0, 0);
			return (1);
		}
		sim->coders[i].last_compile_start = current_time;
		sim->coders[i].sim = sim;
	}
	return (0);
}

static int	init_single_dongle(t_sim *sim, int i)
{
	sim->dongles[i].wait_queue.data = NULL;
	if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
	{
		cleanup_partial_init(sim, sim->params.number_of_coders, i, 0);
		return (1);
	}
	if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		cleanup_partial_init(sim, sim->params.number_of_coders, i, 0);
		return (1);
	}
	sim->dongles[i].is_available = 1;
	sim->dongles[i].released_at = 0;
	if (heap_init(&sim->dongles[i].wait_queue,
			sim->params.number_of_coders))
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		cleanup_partial_init(sim, sim->params.number_of_coders, i, 0);
		return (1);
	}
	return (0);
}

static int	init_dongles(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < sim->params.number_of_coders)
	{
		if (init_single_dongle(sim, i))
			return (1);
	}
	return (0);
}

static int	init_resources(t_sim *sim)
{
	if (init_coders(sim))
		return (1);
	if (init_dongles(sim))
		return (1);
	if (init_global_mutexes(sim))
		return (1);
	sim->simulation_over = 0;
	sim->fifo_counter = 0;
	sim->start_time = get_time_ms();
	return (0);
}

int	init_simulation(t_sim *sim)
{
	sim->coders = malloc(sim->params.number_of_coders * sizeof(t_coder));
	if (!sim->coders)
		return (-1);
	sim->dongles = malloc(sim->params.number_of_coders * sizeof(t_dongle));
	if (!sim->dongles)
	{
		free(sim->coders);
		return (-1);
	}
	if (init_resources(sim))
		return (-1);
	return (0);
}
