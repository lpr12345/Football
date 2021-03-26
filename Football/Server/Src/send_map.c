/*************************************************************************
	> File Name: send_map.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 17时04分12秒
 ************************************************************************/

#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../Include/write_log.h"
#include "../../Thread_pool/thread_pool.h"

User *rteam, *bteam, *wteam;
    
extern pthread_t Re_ball, Send_map;

extern pthread_mutex_t mem_lock;
extern pthread_rwlock_t ball_lock;

extern Bpoint ball;
extern Score score;

extern FILE *rf;
extern char *error_log_path;
extern Thread_pool pool;
extern int time_val;
extern int over_flag;
Exit_user_msg exit_user_msg;

void send_to_user(FootBallMsg *msg, int flag) {
    if (flag == ALL_USER) {
        for (int i = 0; i < MAX; i++) {
            if (rteam[i].online) {
                if (send(rteam[i].fd, (char *)msg, sizeof(FootBallMsg), 0) < 0) {
                    write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
                }
            }
            if (bteam[i].online) {
                if (send(bteam[i].fd, (char *)msg, sizeof(FootBallMsg), 0) < 0) {
                    write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
                }
            }
            if (wteam[i].online) {
                if (send(wteam[i].fd, (char *)msg, sizeof(FootBallMsg), 0) < 0) {
                    write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
                }
            }
        }

        return ;
    } 

    User *temp_team = NULL;
    if (flag == BLUE_USER) temp_team = bteam;
    else if (flag == RED_USER) temp_team = rteam;
    else if (flag == WATCH_USER) temp_team = wteam;
    
    for (int i = 0; i < MAX; i++) {
        if (temp_team[i].online) {
            if (send(temp_team[i].fd, (char *)msg, sizeof(FootBallMsg), 0) < 0) {
                write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
            }
        }
    }

    return ;
}

void *send_map(void *arg) {
    while (1) {
        FootBallMsg msg;
        msg.flag = 0;
        msg.music = 0;
        msg.type = FT_GAME;
    
        for (int i = 0; i < MAX; i++) {
            if (!msg.rteam[i].online && !msg.bteam[i].online) {
                msg.rteam[i].online = rteam[i].online;
                msg.bteam[i].online = bteam[i].online;
                continue;
            }
            msg.rteam[i].team = rteam[i].team;
            msg.rteam[i].online = rteam[i].online;
            msg.rteam[i].loc.x = rteam[i].loc.x;
            msg.rteam[i].loc.y = rteam[i].loc.y;
            strcpy(msg.rteam[i].id, rteam[i].id);

            msg.bteam[i].team = bteam[i].team;
            msg.bteam[i].online = bteam[i].online;
            msg.bteam[i].loc.x = bteam[i].loc.x;
            msg.bteam[i].loc.y = bteam[i].loc.y;
            strcpy(msg.bteam[i].id, bteam[i].id);
        }

        pthread_rwlock_rdlock(&ball_lock);
        msg.ball.x = (int)ball.x;
        msg.ball.y = (int)ball.y;
        pthread_rwlock_unlock(&ball_lock);
        
        msg.score.red = score.red;
        msg.score.blue = score.blue;
        
        send_to_user(&msg, ALL_USER);
        usleep(100000);
    }
}

void send_time() {
    if (over_flag) return ;
    FootBallMsg msg;
    msg.type = FT_TIME;
    msg.flag = time_val;
    msg.music = 0;
    sprintf(msg.msg, "%04d", time_val);
    
    send_to_user(&msg, ALL_USER);

    if (time_val == 0) {
        msg.type = FT_FIN;
        msg.flag = 2;
        msg.music = 2;
        if (score.red > score.blue) {
            add_task(&pool, write_game, "Time out. RED team win.");
            
            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 你赢了!", score.red, score.blue);
            send_to_user(&msg, RED_USER);
            
            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 你输了.", score.red, score.blue);
            send_to_user(&msg, BLUE_USER);
            
            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 红队胜.", score.red, score.blue);
            send_to_user(&msg, WATCH_USER);
        } else if (score.blue > score.red) {
            add_task(&pool, write_game, "Time out. BLUE team win.");
            
            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 你赢了!", score.red, score.blue);
            send_to_user(&msg, BLUE_USER);

            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 你输了.", score.red, score.blue);
            send_to_user(&msg, RED_USER);
            
            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 蓝队胜.", score.red, score.blue);
            send_to_user(&msg, WATCH_USER);
        } else {
            add_task(&pool, write_game, "Time out. RED team BLUE team tied.");
            
            sprintf(msg.msg, "游戏结束! 红队 : 蓝队 -> %d : %d 双方平局..", score.red, score.blue);
            send_to_user(&msg, ALL_USER);
        }

        pthread_cancel(Re_ball);
        pthread_cancel(Send_map);
        
        memcpy(&exit_user_msg.rteam, rteam, sizeof(exit_user_msg.rteam));
        memcpy(&exit_user_msg.bteam, bteam, sizeof(exit_user_msg.bteam));
        
        for (int i = 0; i < MAX; i++) {
            if (rteam[i].online) {
                if (send(rteam[i].fd, (char *)&exit_user_msg, sizeof(Exit_user_msg), 0) < 0) {
                    write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
                }
            }
            if (bteam[i].online) {
                if (send(bteam[i].fd, (char *)&exit_user_msg, sizeof(Exit_user_msg), 0) < 0) {
                    write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
                }
            }
            if (wteam[i].online) {
                if (send(wteam[i].fd, (char *)&exit_user_msg, sizeof(Exit_user_msg), 0) < 0) {
                    write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
                }
            }
        }
        
        sprintf(msg.msg, "Game over. RED : BLUE -> %d : %d", score.red, score.blue);
        printf("%s\n", msg.msg);
        write_game(msg.msg);
        over_flag = 1;
    }

    time_val--;
    return ;
}
