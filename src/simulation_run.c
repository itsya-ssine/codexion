/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_run.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:44:01 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:44:02 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	join_all_threads(t_sim *sim, int created_coders)
{
	int	i;

	i = 0;
	while (i < created_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_mutex_lock(&sim->stop_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_join(sim->monitor_thread, NULL);
	cleanup_simulation(sim);
}

static int	handle_thread_creation_failure(t_sim *sim, int i)
{
	fprintf(stderr, "Error: Failed to create coder thread\n");
	pthread_mutex_lock(&sim->stop_mutex);
	sim->simulation_over = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	pthread_mutex_unlock(&sim->start_mutex);
	join_all_threads(sim, i);
	return (1);
}

int	launch_simulation(t_sim *sim)
{
	int	i;

	pthread_mutex_lock(&sim->start_mutex);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		fprintf(stderr, "Error: Failed to create monitor thread\n");
		pthread_mutex_unlock(&sim->start_mutex);
		cleanup_simulation(sim);
		return (1);
	}
	i = -1;
	while (++i < sim->params.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
			return (handle_thread_creation_failure(sim, i));
	}
	sim->start_time = get_time_ms();
	pthread_mutex_unlock(&sim->start_mutex);
	join_all_threads(sim, i);
	return (0);
}
