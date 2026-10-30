/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:43:48 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:43:50 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_global_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL))
	{
		cleanup_partial_init(sim, sim->params.number_of_coders,
			sim->params.number_of_coders, 0);
		return (1);
	}
	if (pthread_mutex_init(&sim->stop_mutex, NULL))
	{
		cleanup_partial_init(sim, sim->params.number_of_coders,
			sim->params.number_of_coders, 1);
		return (1);
	}
	if (pthread_mutex_init(&sim->start_mutex, NULL))
	{
		cleanup_partial_init(sim, sim->params.number_of_coders,
			sim->params.number_of_coders, 2);
		return (1);
	}
	if (pthread_mutex_init(&sim->fifo_mutex, NULL))
	{
		cleanup_partial_init(sim, sim->params.number_of_coders,
			sim->params.number_of_coders, 3);
		return (1);
	}
	return (0);
}
