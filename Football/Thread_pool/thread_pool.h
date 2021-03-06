/*************************************************************************
	> File Name: thread_pool.h
	> Author: 
	> Mail: 
	> Created Time: 2020年08月10日 星期一 10时25分55秒
 ************************************************************************/


#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "../Common/head.h"

typedef struct Task {
    void *(*run)(void *arg);
    void *arg;
} Task;

typedef struct Thread_pool {
    int cnt;
    int wait_cnt;
    int max_thread_cnt;
    int exit_flag;
    int head;
    int tail;
    int tasks_cnt;
    pthread_cond_t have_task;
    pthread_cond_t over;
    pthread_mutex_t mutex;
    Task *tasks;
    pthread_t *threads;
} Thread_pool;

void *thread_run(void *arg);

bool thread_is_alive(pthread_t thread);

int add_task(Thread_pool *pool, void *(*run)(void *args), void *arg);

void thread_pool_clear(Thread_pool *pool);

void thread_pool_init(Thread_pool *pool);

#endif
