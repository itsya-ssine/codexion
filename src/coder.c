/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:44:21 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:44:22 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	wait_and_reset_timer(t_coder *coder)
{
	pthread_mutex_lock(&coder->coder_mutex);
	while (!is_sim_over(coder->sim) && get_time_ms()
		- coder->last_compile_start > coder->sim->params.time_to_burnout)
	{
		pthread_mutex_unlock(&coder->coder_mutex);
		usleep(100);
		pthread_mutex_lock(&coder->coder_mutex);
	}
	if (is_sim_over(coder->sim))
	{
		pthread_mutex_unlock(&coder->coder_mutex);
		return (1);
	}
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->coder_mutex);
	return (0);
}

static int	perform_compile_cycle(t_coder *coder, t_dongle *first,
		t_dongle *second)
{
	t_sim	*sim;

	sim = coder->sim;
	if (wait_and_reset_timer(coder))
	{
		dongle_release(second);
		dongle_release(first);
		return (1);
	}
	log_event(sim, coder->id, "has taken a dongle");
	log_event(sim, coder->id, "has taken a dongle");
	log_event(sim, coder->id, "is compiling");
	ft_msleep(sim->params.time_to_compile, sim);
	pthread_mutex_lock(&coder->coder_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->coder_mutex);
	dongle_release(second);
	dongle_release(first);
	return (0);
}

static void	coder_main_loop(t_coder *coder, t_dongle *first, t_dongle *second)
{
	long long	priority;

	if (coder->sim->params.number_of_compiles_required == 0)
		return ;
	while (!is_sim_over(coder->sim))
	{
		priority = get_next_priority(coder, coder->sim);
		if (dongles_acquire(first, second, priority, coder))
			break ;
		if (perform_compile_cycle(coder, first, second))
			break ;
		log_event(coder->sim, coder->id, "is debugging");
		ft_msleep(coder->sim->params.time_to_debug, coder->sim);
		log_event(coder->sim, coder->id, "is refactoring");
		ft_msleep(coder->sim->params.time_to_refactor, coder->sim);
	}
}

void	*coder_routine(void *arg)
{
	t_coder		*c;
	t_dongle	*left;
	t_dongle	*right;

	c = (t_coder *)arg;
	if (wait_for_simulation_start(c))
		return (NULL);
	if (c->sim->params.number_of_coders == 1)
	{
		dongle_acquire(&c->sim->dongles[0], get_next_priority(c, c->sim), c);
		ft_msleep(c->sim->params.time_to_burnout + 10, c->sim);
		dongle_release(&c->sim->dongles[0]);
		return (NULL);
	}
	left = &c->sim->dongles[c->id - 1];
	right = &c->sim->dongles[c->id % c->sim->params.number_of_coders];
	if (c->id % 2 == 0)
	{
		ft_msleep((c->sim->params.time_to_compile
				+ c->sim->params.dongle_cooldown) / 4, c->sim);
		coder_main_loop(c, left, right);
	}
	else
		coder_main_loop(c, right, left);
	return (NULL);
}
