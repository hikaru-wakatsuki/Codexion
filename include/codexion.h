/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hwakatsu <hwakatsu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 22:55:56 by hwakatsu          #+#    #+#             */
/*   Updated: 2026/04/26 18:46:26 by hwakatsu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/time.h>

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
	int					left_dongle_index;
	int					right_dongle_index;
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
	bool				stop;
	int					finished_count;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		finish_mutex;
	pthread_mutex_t		log_mutex;
	pthread_t			monitor_thread;
	t_coder				*coders;
	t_dongle			*dongles;
	long				request_seq;
	pthread_mutex_t		seq_mutex;
}						t_sim;

#endif
