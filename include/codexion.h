/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelmajdo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 23:43:20 by yelmajdo          #+#    #+#             */
/*   Updated: 2026/08/28 23:43:21 by yelmajdo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

// includes
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

// scheduler policy
typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}						t_scheduler;

// data structures
typedef struct s_coder	t_coder;

typedef struct s_node
{
	long long			priority;
	int					compile_count;
	int					coder_id;
}						t_node;

typedef struct s_heap
{
	t_node		*data;
	int			size;
	int			capacity;
}						t_heap;

typedef struct s_dongle
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	int					is_available;
	long long			released_at;
	t_heap				wait_queue;
}						t_dongle;

typedef struct s_params
{
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	long long			dongle_cooldown;
	int					number_of_coders;
	int					number_of_compiles_required;
	int					scheduler;
}						t_params;

typedef struct s_sim
{
	t_params			params;
	t_coder				*coders;
	t_dongle			*dongles;
	pthread_mutex_t		log_mutex;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		start_mutex;
	int					simulation_over;
	long long			start_time;
	long long			fifo_counter;
	pthread_mutex_t		fifo_mutex;
	pthread_t			monitor_thread;
}						t_sim;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	long long			last_compile_start;
	int					compile_count;
	pthread_mutex_t		coder_mutex;
	t_sim				*sim;
}						t_coder;

// time_utils.c
long long				get_time_ms(void);
void					set_target_timespec(long long target,
							struct timespec *ts);
void					ft_msleep(long long ms, t_sim *sim);
int						is_sim_over(t_sim *sim);

// validation.c
int						is_valid_number(const char *str);

// heap.c
int						heap_init(t_heap *h, int capacity);
void					sift_up(t_heap *h, int i);
void					sift_down(t_heap *h, int i);

// heap_operations.c
int						heap_push(t_heap *h, long long priority,
							int compile_count, int coder_id);
t_node					heap_pop(t_heap *h);
int						heap_peek_id(t_heap *h);

// logger.c
void					log_event(t_sim *sim, int coder_id, char *event);

// args_parser.c
int						parse_arguments(t_sim *sim, int argc, char **argv);

// simulation_init.c
int						init_simulation(t_sim *sim);

// mutex_utils.c
int						init_global_mutexes(t_sim *sim);

// simulation_cleanup.c
void					cleanup_partial_init(t_sim *sim, int c_done,
							int d_done, int g_done);
void					cleanup_simulation(t_sim *sim);

// simulation_run.c
int						launch_simulation(t_sim *sim);

// dongle.c
int						dongle_acquire(t_dongle *dongle, long long priority,
							t_coder *coder);
int						dongles_acquire(t_dongle *first, t_dongle *second,
							long long priority, t_coder *coder);
void					dongle_release(t_dongle *dongle);

// coder_helpers.c
long long				get_next_priority(t_coder *coder, t_sim *sim);
int						wait_for_simulation_start(t_coder *c);

// coder.c
void					*coder_routine(void *arg);

// monitor.c
void					*monitor_routine(void *arg);

#endif
