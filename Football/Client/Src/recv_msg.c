/*************************************************************************
	> File Name: recv_msg.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 17时50分58秒
 ************************************************************************/

#include "../../Common/head.h"
#include "../../Common/datatype.h"
#include "../Include/game_map.h"
#include "../Include/write_log.h"
#include "../Include/music.h"
#include "../Include/draw_game.h"

extern User_t *rteam;
extern User_t *bteam;

extern FootBallMsg root_user;
extern int sockfd;
extern Point ball;
extern Score score;
extern pthread_mutex_t draw_lock;
extern WINDOW *Time_win, *Score_win, *Football_win;
extern char *error_log_path;
extern int exit_flag;
extern Map court;
extern int sound_effect_flag;

extern Watch_user watch_user[MAX];

extern pthread_t C_RECV;
extern pthread_t Re_draw_t;
extern pthread_t Send_ctl;

Exit_user_msg exit_user_msg;

int find_ind() {
    for (int i = 0; i < MAX; i++) {
        if (!watch_user[i].online) return i;
    }
    return -1;
}

void *client_recv(void *arg) {
    while (1) {
        FootBallMsg msg;
        memset(&msg, 0, sizeof(msg));
        recv(sockfd, &msg, sizeof(msg), 0);
        if (sound_effect_flag) {
            if (msg.music) {
                pthread_t Play_music;
                pthread_create(&Play_music, NULL, play_music, (void *)&msg.music);
            }
        }
        if (msg.type & FT_TEST) {
            msg.type = FT_ACK;
            if (send(sockfd, (char *)&msg, sizeof(msg), 0) < 0) {
                write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));   
            }
        } else if (msg.type & FT_MSG) {
            show_message(&msg, msg.msg, 0);
        } else if (msg.type & FT_WALL) {
            if (msg.flag == 1) {
                int ind = find_ind();
                if (ind == -1) return NULL;
                watch_user[ind].online = 1;
                strcpy(watch_user[ind].name, msg.name);

                re_draw_watch();
            }
            
            if (msg.flag == 2) {
                for (int i = 0; i < MAX; i++) {
                    if (strcmp(watch_user[i].name, msg.name) == 0) {
                        watch_user[i].online = 0;
                        break;
                    }
                }
                re_draw_watch();
            }

            show_message(&root_user, msg.msg, 1);
        } else if (msg.type & FT_FIN) {
            if(msg.flag == 1) { 
                show_message(&root_user, "服务端主动退出.强制关闭游戏.抱歉.", 1);
                show_message(&root_user, "您可以继续听音乐.", 1);
                exit_flag = 1;
                pthread_cancel(Re_draw_t);
                pthread_cancel(Send_ctl);
                draw_exit_win(1);
                break;
            } else {
                show_message(&root_user, "游戏结束.您可以继续聊天听音乐.", 1);

                recv(sockfd, &exit_user_msg, sizeof(Exit_user_msg), 0);

                exit_flag = 2;
                pthread_cancel(Re_draw_t);
                draw_exit_win(2);
                
                wattron(Football_win, COLOR_PAIR(4));
                w_gotoxy_football_s_com(court.width - 20 , court.start.y + 2, msg.msg);
                    
                wrefresh(Football_win);
                
                draw_exit_user(&exit_user_msg);
            }
        } else if (msg.type & FT_GAME) {
            memcpy(rteam, &msg.rteam, sizeof(User_t) * MAX);
            memcpy(bteam, &msg.bteam, sizeof(User_t) * MAX);
            ball.x = msg.ball.x;
            ball.y = msg.ball.y;
            if (score.red != msg.score.red || score.blue != msg.score.blue) {
                score.red = msg.score.red;
                score.blue = msg.score.blue;
                
                char temp_str_score[20];
                sprintf(temp_str_score, "红队得分 : %d", score.red);
                pthread_mutex_lock(&draw_lock);
                wattron(Score_win, COLOR_PAIR(2));
                w_gotoxy_puts(Score_win, 3, 2, temp_str_score);
                sprintf(temp_str_score, "蓝队得分 : %d", score.blue);
                wattron(Score_win, COLOR_PAIR(6));
                w_gotoxy_puts(Score_win, 3, 5, temp_str_score);
                pthread_mutex_unlock(&draw_lock);
            }
        } else if (msg.type & FT_TIME) {
            pthread_mutex_lock(&draw_lock);
            w_gotoxy_puts(Time_win, 8, 3, msg.msg);
            pthread_mutex_unlock(&draw_lock);
        } 
    }
}

