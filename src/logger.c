/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:43:42 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:43:44 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_event(t_sim *sim, int coder_id, char *event)
{
	long long	time_spame;

	pthread_mutex_lock(&sim->log_mutex);
	if (is_sim_over(sim))
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	time_spame = get_time_ms() - sim->start_time;
	printf("%lld %d %s\n", time_spame, coder_id, event);
	pthread_mutex_unlock(&sim->log_mutex);
}
