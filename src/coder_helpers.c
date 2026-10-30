/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:44:49 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:44:51 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_next_priority(t_coder *coder, t_sim *sim)
{
	long long	priority;

	if (sim->params.scheduler == SCHEDULER_EDF)
	{
		pthread_mutex_lock(&coder->coder_mutex);
		priority = coder->last_compile_start + sim->params.time_to_burnout;
		pthread_mutex_unlock(&coder->coder_mutex);
	}
	else
	{
		pthread_mutex_lock(&sim->fifo_mutex);
		priority = sim->fifo_counter++;
		pthread_mutex_unlock(&sim->fifo_mutex);
	}
	return (priority);
}

int	wait_for_simulation_start(t_coder *c)
{
	pthread_mutex_lock(&c->sim->start_mutex);
	pthread_mutex_unlock(&c->sim->start_mutex);
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_start = c->sim->start_time;
	pthread_mutex_unlock(&c->coder_mutex);
	return (is_sim_over(c->sim));
}
