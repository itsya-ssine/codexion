/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_cleanup.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:44:37 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:44:38 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_partial_init(t_sim *sim, int c_done, int d_done, int g_done)
{
	int	i;

	if (!sim)
		return ;
	i = -1;
	while (++i < c_done)
		pthread_mutex_destroy(&sim->coders[i].coder_mutex);
	i = -1;
	while (++i < d_done)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		free(sim->dongles[i].wait_queue.data);
	}
	if (g_done >= 4)
		pthread_mutex_destroy(&sim->fifo_mutex);
	if (g_done >= 3)
		pthread_mutex_destroy(&sim->start_mutex);
	if (g_done >= 2)
		pthread_mutex_destroy(&sim->stop_mutex);
	if (g_done >= 1)
		pthread_mutex_destroy(&sim->log_mutex);
}

void	cleanup_simulation(t_sim *sim)
{
	int	j;

	j = 0;
	while (j < sim->params.number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[j].coder_mutex);
		pthread_mutex_destroy(&sim->dongles[j].mutex);
		pthread_cond_destroy(&sim->dongles[j].cond);
		free(sim->dongles[j].wait_queue.data);
		j++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->start_mutex);
	pthread_mutex_destroy(&sim->fifo_mutex);
	free(sim->coders);
	free(sim->dongles);
}
