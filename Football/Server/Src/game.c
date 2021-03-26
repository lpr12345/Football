/*************************************************************************
	> File Name: game.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 11时42分29秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../Include/write_log.h"
#include "../../Thread_pool/thread_pool.h"
#include "../Include/send_map.h"

extern Map court;

extern BallStatus ball_status;
extern User *rteam, *bteam;
extern Bpoint ball;
extern Score score;
extern FootBallMsg root_user;

extern pthread_rwlock_t ball_lock;
extern pthread_mutex_t status_lock;

extern char *error_log_path;

extern Thread_pool pool;

void ball_zero() {
    pthread_mutex_lock(&status_lock);
    ball_status.dir.x = 0;
    ball_status.dir.y = 0;
    ball_status.v = 0;
    ball_status.a = 0;
    pthread_mutex_unlock(&status_lock);
}

void *send_goal(void *arg) {
    static int temp_score_r = 0, temp_score_b = 0;
    FootBallMsg temp_msg;
    temp_msg.flag = 0;
    temp_msg.music = 0;
    if (ball_status.by_team) {
        if (score.blue - temp_score_b) {
            temp_score_b++;
            temp_msg.music = 6;
            sprintf(temp_msg.msg, "%s 踢进一球! 蓝队得一分! 球权转交红队.", ball_status.by_user->name);
            ball_status.by_user->score++;
            ball_status.flag_team = 0;
        } else {
            temp_score_r++;
            temp_msg.music = 3;
            sprintf(temp_msg.msg, "%s 踢进一个乌龙球, 红队得一分! 蓝队球权.", ball_status.by_user->name);
            ball_status.by_user->own++;
            ball_status.flag_team = 1;
        }
    } else {
        if (score.red - temp_score_r) {
            temp_score_r++;
            temp_msg.music = 6;
            sprintf(temp_msg.msg, "%s 踢进一球! 红队得一分! 球权转交蓝队.", ball_status.by_user->name);
            ball_status.by_user->score++;
            ball_status.flag_team = 1;
        } else  {
            temp_score_b++;
            temp_msg.music = 3;
            sprintf(temp_msg.msg, "%s 踢进一个乌龙球, 蓝队得一分! 红队球权.", ball_status.by_user->name);
            ball_status.by_user->own++;
            ball_status.flag_team = 0;
        }
    }
    
    add_task(&pool, write_game, (void *)&temp_msg.msg);

    temp_msg.type = FT_WALL;
    send_to_user(&temp_msg, ALL_USER);
}

void out_ball() {
    pthread_mutex_lock(&status_lock);
    ball_status.flag_team = !ball_status.by_team;
    ball_status.out_flag = 1;
    pthread_mutex_unlock(&status_lock);

    FootBallMsg temp_msg;
    if (ball_status.by_team) sprintf(temp_msg.msg, "%s 踢出界了. 球权转交红队.", ball_status.by_user->name);
    else sprintf(temp_msg.msg, "%s 踢出界了. 球权转交蓝队.", ball_status.by_user->name);
    
    add_task(&pool, write_game, (void *)&temp_msg.msg);
    
    temp_msg.type = FT_WALL;
    temp_msg.music = 7;
    temp_msg.flag = 0;
    send_to_user(&temp_msg, ALL_USER);
    return ;
}

void *re_ball(void *arg) {
    while (1) {
        if (ball_status.v != 0) {
            pthread_rwlock_wrlock(&ball_lock);
            int last_bx = (int)ball.x;
            int last_by = (int)ball.y;
            ball.x = ball.x + ball_status.dir.x * (ball_status.v * 0.1 + ball_status.a * 0.5 * 0.01);
            ball.y = ball.y + ball_status.dir.y * (ball_status.v * 0.1 + ball_status.a * 0.5 * 0.01);
            int bx = (int)ball.x;
            int by = (int)ball.y;
            if (!ball_status.out_flag) {
            //球出左右边界:
                if (bx < 2 || bx > (court.width - 3)) {
                    int mid_y = (court.start.y + court.heigth + 4) / 2 - 3;
                    if (by == (mid_y - 3)) {
                        ball.y = last_by;
                        ball.x = last_bx;
                        by = last_by;
                    } else if (by == (mid_y + 3)) {
                        ball.y = last_by;
                        ball.x = last_bx;
                        by = last_by;
                    }
                    if (by > (mid_y - 3) && by < (mid_y + 3)) {
                        if (bx < 2) score.blue++;
                        else  score.red++;
                    
                        add_task(&pool, send_goal, NULL);

                        ball.x = court.width / 2;  
                        ball.y = court.heigth / 2;  
                        by = court.heigth / 2;
                    } else {
                        if (bx < 1) ball.x = 1;
                        else if (bx > (court.width - 2)) ball.x = court.width - 2;
                        out_ball();
                    }
                    ball_zero();
                }
                //球出上下边界:
                if (by <= 1) {
                    out_ball();
                    ball.y = 1;
                    ball_zero();
                } else if (by >= (court.heigth - 2)) {
                    out_ball();
                    ball.y = court.heigth - 2;
                    ball_zero();
                }
            } else ball_status.out_flag = 0;
            pthread_rwlock_unlock(&ball_lock);
            pthread_mutex_lock(&status_lock);
            ball_status.v += (ball_status.a * 0.1);
            if (ball_status.v < 0) {
                ball_status.v = 0;
                ball_status.a = 0;
            }
            pthread_mutex_unlock(&status_lock);
        }
        
        usleep(100000);  
    }
}

void *send_out_ball(void *arg) {
    FootBallMsg temp_msg;
    temp_msg.type = FT_WALL;
    temp_msg.music = 0;
    temp_msg.flag = 0;
    strcpy(temp_msg.msg, (char *)arg);
    send_to_user(&temp_msg, ALL_USER);
}

int can_kick(Point *loc, int user_team, int strength) { 
    //判断是否能踢，能的话更新速度
    int px = loc->x, py = loc->y;
    pthread_rwlock_rdlock(&ball_lock);
    if (abs(px - (int)ball.x) <= 1 && abs(py - (int)ball.y) <= 1) {
        pthread_rwlock_unlock(&ball_lock);
        if (ball_status.flag_team == !user_team) {
            char temp_str[50];
            if (ball_status.flag_team) sprintf(temp_str, "蓝队球权.");
            else sprintf(temp_str, "红队球权.");

            add_task(&pool, send_out_ball, (void *)temp_str);
            return 0;
        }

        pthread_mutex_lock(&status_lock);
        ball_status.flag_team = 2;
        if (px == (int)ball.x && py == (int)ball.y) {
            ball_status.dir.x = 0;
            ball_status.dir.y = 0;
            pthread_mutex_unlock(&status_lock);
            return 0;
        }
        if ((int)ball.x - px < 0) ball_status.dir.x = -1;
        else if ((int)ball.x - px > 0) ball_status.dir.x = 1;
        else ball_status.dir.x = 0;
        if ((int)ball.y - py < 0) ball_status.dir.y = -1;
        else if ((int)ball.y - py > 0) ball_status.dir.y = 1;
        else ball_status.dir.y = 0;
        ball_status.a = -10;
        ball_status.v = strength;
        if (ball_status.dir.x != 0 && ball_status.dir.y != 0) ball_status.v *= 0.75;
        pthread_mutex_unlock(&status_lock);
        
        return 1;
    }
    pthread_rwlock_unlock(&ball_lock);
    return 0;
}

int can_stop(Point *loc) {
    //判断是否能停球
    int px = loc->x, py = loc->y;
    pthread_rwlock_rdlock(&ball_lock);
    if (abs(px - (int)ball.x) > 1 || abs(py - (int)ball.y > 1)) {
            pthread_rwlock_unlock(&ball_lock);
            return 0;
    }
    pthread_rwlock_unlock(&ball_lock);
    ball_zero();
    return 1;
}
