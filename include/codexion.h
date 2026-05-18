/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 22:55:56 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/05/18 16:40:29 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define MAX_CODERS 1000

typedef struct s_sim	t_sim;

typedef enum e_scheduler
{
	FIFO,
	EDF,
}						t_scheduler;

typedef struct s_request
{
	int					coder_id;
	long				arrival_seq;
	long				deadline_ms;
}						t_request;

typedef struct s_heap
{
	t_request			*data;
	int					size;
	int					capacity;
}						t_heap;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	bool				is_thread_created;
	int					left_dongle_idx;
	int					right_dongle_idx;
	long				last_compile_start_ms;
	int					compile_count;
	pthread_mutex_t		state_mutex;
	t_sim				*sim;
}						t_coder;

typedef struct s_dongle
{
	int					id;
	int					owner_coder_id;
	long				cooldown_until_ms;
	long				local_seq;
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	t_heap				wait_queue;
}						t_dongle;

typedef struct s_sim
{
	int					n_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	long				dongle_cooldown;

	int					must_compile_count;
	t_scheduler			scheduler;
	long				start_ms;
	bool				stop_simulation;
	int					finished_count;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		finish_mutex;
	pthread_mutex_t		log_mutex;
	pthread_t			monitor_thread;
	bool				is_monitor_created;
	t_coder				*coders;
	t_dongle			*dongles;
}						t_sim;

// coder_routine.c
bool					is_finished(t_coder *coder);
void					*coder_routine(void *arg);
// dongles_release.c
void					release_dongle(t_coder *coder, t_dongle *dongle);
void					release_dongles(t_coder *coder);
// dongles_take.c
bool					take_dongles(t_coder *coder);
bool					is_higher_priority(t_request a, t_request b,
							t_sim *sim);
// heap.c
bool					push_request(t_heap *heap, t_request req, t_sim *sim);
t_request				pop_request(t_heap *heap, t_sim *sim);
// main.c
int						main(int argc, char *argv[]);
// monitor_routine.c
void					*monitor_routine(void *arg);
// parse_args.c
bool					parse_args(int argc, char *argv[], t_sim *sim);
// sim_control.c
bool					init_sim(t_sim *sim);
bool					is_stopped(t_sim *sim);
void					cleanup_sim(t_sim *sim);
// threads_control.c
bool					start_threads(t_sim *sim);
void					join_threads(t_sim *sim);
// time.c
long					get_time_ms(void);
void					print_log(t_sim *sim, int id, char *msg);
void					smart_sleep(long ms, t_sim *sim);
#endif
