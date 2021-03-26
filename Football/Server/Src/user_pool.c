/*************************************************************************
	> File Name: user_pool.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月04日 星期三 18时10分31秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../Include/user_pool.h"
#include "../../Common/common.h"
#include "../Include/udp_server.h"
#include "../Include/game.h"
#include "../Include/write_log.h"
#include "../../Thread_pool/thread_pool.h"
#include "../Include/send_map.h"


extern int bepollfd, repollfd, wepollfd;
extern FootBallMsg root_user;
extern Map court; 
extern User *rteam, *bteam, *wteam;
extern BallStatus ball_status;
extern int flag_arr[50][100];
extern int flag_id[26][26];
char write_buff[50];

extern char *error_log_path;

extern Thread_pool pool;

void *do_echo(void *e_m) {
    Echo_user_msg *echo_user_msg = (Echo_user_msg *)e_m;
    User *user = echo_user_msg->user;
    FootBallMsg msg;
    msg.music = 0;
    msg.flag = 0;
    memcpy(&msg, &echo_user_msg->foot_ball_msg, sizeof(FootBallMsg));
    free(echo_user_msg);
    user->flag = 3;
    if (msg.type & FT_MSG) {
        strcpy(msg.name, user->name);
        msg.team = user->team;
        if (msg.flag == 1) {
            msg.music = 4;
            send_to_user(&msg, ALL_USER);
        } else if (msg.flag == 2) {
            msg.music = 4;
            if (msg.team == 1) send_to_user(&msg, BLUE_USER);
            else if (msg.team == 0) send_to_user(&msg, RED_USER);
            else send_to_user(&msg, WATCH_USER);
        } else if (msg.flag == 3) {
            int temp_fd = 0;
            for (int i = 0; i < MAX; i++) {
                if (!strcmp(rteam[i].name, msg.to_name) && rteam[i].online) {
                    temp_fd = rteam[i].fd;    
                } else if (!strcmp(bteam[i].name, msg.to_name) && bteam[i].online) {
                    temp_fd = bteam[i].fd;    
                } else if (!strcmp(wteam[i].name, msg.to_name) && wteam[i].online) {
                    temp_fd = wteam[i].fd;    
                }
            }

            if (temp_fd == 0) {
                msg.type = FT_WALL;
                sprintf(msg.msg, "抱歉.找不到这个用户 %s.", msg.to_name);
                send(user->fd, (char *)&msg, sizeof(msg), 0);
            
                return NULL;
            }

            if(temp_fd == user->fd) {
                msg.type = FT_WALL;
                sprintf(msg.msg, "你不能对自己私聊.");
                send(user->fd, (char *)&msg, sizeof(msg), 0);
                        
                return NULL;
            } 
                    
            msg.music = 5;
            msg.type = FT_MSG;
            msg.flag = 3;
            send(temp_fd, (char *)&msg, sizeof(FootBallMsg), 0);
                    
            msg.type = FT_WALL;
            char temp[256];
            strcpy(temp, msg.msg);
            sprintf(msg.msg, "你对 %s 说 : %s.", msg.to_name, temp);
            send(user->fd, (char *)&msg, sizeof(msg), 0);
        }
    } else if (msg.type & FT_FIN) {
        sprintf(write_buff, "%s logout.", user->name);
        add_task(&pool, write_game, (void *)&write_buff);
        
        user->online = 0;

        msg.type = FT_WALL;
        sprintf(msg.msg, "%s 退出游戏了.", user->name);
        msg.music = 2;
        
        if (user->team == 2) {
            msg.flag = 2;
            strcpy(msg.name, user->name);
            send_to_user(&msg, ALL_USER);
            del_event(wepollfd, user->fd);
            return NULL;
        }

        flag_arr[user->loc.y][user->loc.x] = 0;
        flag_id[user->id[0] - 'A'][user->id[1] - 'A'] = 0;
        
        send_to_user(&msg, ALL_USER);
        int epollfd_tmp = (user->team ? bepollfd : repollfd);
        del_event(epollfd_tmp, user->fd);
    } else if (msg.type & FT_CTL) {
        if (msg.flag == 1) {
            if (msg.ctl.dirx || msg.ctl.diry) {
                int temp_x = user->loc.x + msg.ctl.dirx;
                int temp_y = user->loc.y + msg.ctl.diry;
                if (flag_arr[temp_y][temp_x]) {
                    
                    return NULL;
                }
                flag_arr[user->loc.y][user->loc.x] = 0;
                flag_arr[temp_y][temp_x] = 1;
                user->loc.x = temp_x;
                user->loc.y = temp_y;
                if (user->loc.x < 1) user->loc.x = 0;
                if (user->loc.x >= court.width - 1) user->loc.x = court.width - 1;
                if (user->loc.y < 1) user->loc.y = 0;
                if (user->loc.y >= court.heigth - 1) user->loc.y = court.heigth - 1;
            }
        } else if (msg.flag == 2) {
            if (can_kick(&user->loc, user->team, msg.strength)) {
                ball_status.by_team = user->team;
                ball_status.by_user = user;
            }
        } else if (msg.flag == 3) {
            can_stop(&user->loc);
        }
    }

    return NULL;
}

void task_queue_init(struct task_queue *q, int sum, int epollfd) {
    q->sum = sum;
    q->epollfd = epollfd;
    q->team = calloc(sum, sizeof(void *));
    q->head = q->tail = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return ;
}

void task_queue_push(struct task_queue*q, Echo_user_msg *echo_user_msg) {
    pthread_mutex_lock(&q->mutex);
    q->team[q->tail] = echo_user_msg;

    if (++q->tail == q->sum) {
        q->tail = 0;
    }
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return ;
}

Echo_user_msg *task_queue_pop(struct task_queue *q) {
    pthread_mutex_lock(&q->mutex);
    while (q->tail == q->head) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    Echo_user_msg *echo_user_msg = q->team[q->head];

    if (++q->head == q->sum) {
        q->head = 0;
    }
    pthread_mutex_unlock(&q->mutex);
    return echo_user_msg;
}

void *thread_go(void *arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);
    struct task_queue *q = (struct task_queue *)arg;
    while (1) {
        Echo_user_msg *echo_user_msg;
        echo_user_msg = task_queue_pop(q);
        
        add_task(&pool, do_echo, (void *)echo_user_msg);
    }
}

