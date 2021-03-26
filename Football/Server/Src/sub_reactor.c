/*************************************************************************
	> File Name: sub_reactor.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 12时35分54秒
 ************************************************************************/

#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../../Common/common.h"
#include "../Include/user_pool.h"
#include "../Include/udp_server.h"
#include "../Include/write_log.h"
#include "../../Thread_pool/thread_pool.h"

extern User *rteam, *bteam, *wteam;
extern int repollfd, bepollfd, wepollfd;
extern int flag_id[26][26];

extern char *error_log_path;

extern Thread_pool pool;

int find_ind(User *team) {
    for (int i = 0; i < MAX; i++) {
        if (!team[i].online) return i;
    }
    return -1;
}

void add_to_sub_reactor(User *user) {
    User *team;
    switch (user->team) {
        case 0 : team = rteam;
        break;
        case 1 : team = bteam;
        break;
        case 2 : team = wteam;
        break;
    }
    int ind = find_ind(team);
    if (ind < 0) {
        add_task(&pool, write_game, "User is full.");
        return ;
    }
     
    team[ind] = *user;
    team[ind].online = 1;
    team[ind].flag = 3;

    if (user->team == 1) add_event_ptr(bepollfd, team[ind].fd, EPOLLIN, &team[ind]);
    else if (user->team == 0) add_event_ptr(repollfd, team[ind].fd, EPOLLIN, &team[ind]);
    else add_event_ptr(wepollfd, team[ind].fd, EPOLLIN, &team[ind]);
    return ;
}

void *sub_reactor(void *arg) {
    struct task_queue *q = (struct task_queue *)arg;
    pthread_t *tid = (pthread_t *)calloc(MAXTHREAD, sizeof(pthread_t));

    for (int i = 0; i < MAXTHREAD; i++) {
        pthread_create(&tid[i], NULL, thread_go, (void *)q);
    }

    struct epoll_event ev, events[MAX];
    while (1) {
        int nfds = epoll_wait(q->epollfd, events, MAX, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].events & EPOLLIN) {
                Echo_user_msg *echo_user_msg = (Echo_user_msg *)malloc(sizeof(Echo_user_msg));
                echo_user_msg->user = (User *)events[i].data.ptr;

                recv(echo_user_msg->user->fd, &echo_user_msg->foot_ball_msg, sizeof(FootBallMsg), 0);

                task_queue_push(q, echo_user_msg);
            }
        }
    }
}


