/*************************************************************************
	> File Name: user_pool.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月04日 星期三 18时25分08秒
 ************************************************************************/

#ifndef _USER_POOL_H
#define _USER_POOL_H
#include "../../Common/head.h"
#include "../../Common/datatype.h"

typedef struct task_queue {
    int sum;
    int epollfd;
    int head;
    int tail;
    Echo_user_msg **team;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} task_queue;

void task_queue_init(struct task_queue *q, int sum, int epollfd);
void task_queue_push(struct task_queue *q, Echo_user_msg *echo_user_msg);
Echo_user_msg * task_queue_pop(struct task_queue *q);
void *thread_go(void *arg);

#endif
