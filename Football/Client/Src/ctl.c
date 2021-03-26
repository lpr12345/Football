/*************************************************************************
	> File Name: ctl.c
	> Author: 
	> Mail: 
	> Created Time: 2021年01月19日 星期二 17时32分38秒
 ************************************************************************/

#include "../../Common/common.h"
#include "../../Common/datatype.h"
#include "../Include/game_map.h"
#include "../../Common/head.h"
#include "../Include/draw_game.h"
#include "../Include/send_msg.h"

extern int sockfd;
extern int exit_flag;
extern int over_flag;

extern int music_dir;
extern int music_ind;
extern int current_music_id;
extern int bgm_flag;
extern int stop_bgm_flag;

extern FootBallMsg root_user, chat_msg, ctl_msg;

extern pthread_t C_RECV;
extern pthread_t Re_draw_t;
extern pthread_t Send_ctl;

extern pthread_mutex_t stop_music_lock;

void logout() {
    if (!exit_flag) {
        pthread_cancel(Re_draw_t);
        pthread_cancel(C_RECV);
        pthread_cancel(Send_ctl);
    }

    FootBallMsg msg;
    msg.type = FT_FIN;
    send(sockfd, (char *)&msg, sizeof(msg), 0);
    close(sockfd);
    over_flag = 1;
    
    sys_off();

    clear();
    endwin();
    clear();
    exit(0);
    return ;
}

void play_ctl() {
	while (1) {
        int c = getchar();
        switch (c) {
            case 'a' : //向左
                ctl_msg.ctl.dirx = -1;
                break; 
            case 'd' : //向右
                ctl_msg.ctl.dirx = 1;
                break;
            case 'w' : //向上
                ctl_msg.ctl.diry = -1;
                break;
            case 's' : //向下
                ctl_msg.ctl.diry = 1;
                break;
            case 'q' : //向左上
                ctl_msg.ctl.diry = -1;
                ctl_msg.ctl.dirx = -1;
                break;
            case 'e' : //向右上
                ctl_msg.ctl.diry = -1;
                ctl_msg.ctl.dirx = 1;
                break;
            case 'z' : //向左下
                ctl_msg.ctl.diry = 1;
                ctl_msg.ctl.dirx = -1;
                break;
            case 'c' : //向右下
                ctl_msg.ctl.diry = 1;
                ctl_msg.ctl.dirx = 1;
                break;
            case 'k' : //踢球
                send_strength(6);
                break;
            case ' ' : //踢球
                if (exit_flag > 0) break;
                show_strength();
                break;
            case 'j' : //停球
                send_stop_ball(); 
                break;
            case 13 : //输入回车开启输入窗口发消息
                if (exit_flag == 1) break;
                send_chat();
                break;
            case '.' :
                logout(); //退出游戏
                break;
            case '-' :
                music_dir = -1;
                move_music();
                break;
            case '+' :
                music_dir = 1;
                move_music();
                break;
            case '*' :
                if (bgm_flag == 0) {
                    show_message(&root_user, "抱歉，配置文件的bgm开关没有打开.不能播放音乐.", 1);
                    break;
                }
                if (stop_bgm_flag == 1) {
                    stop_bgm_flag = 0;
                    pthread_mutex_unlock(&stop_music_lock);
                }
                current_music_id = music_ind;
                sys_off();
                break;
            case '/' :
                if (stop_bgm_flag == 1) break;
                
                draw_now_music(0); //清除正在播放窗口的音乐
                
                pthread_mutex_lock(&stop_music_lock);
                stop_bgm_flag = 1;
                sys_off();
                break;
            default:
                break;
        }
    }
}

void watch_ctl() {
	while (1) {
        int c = getchar();
        switch (c) {
            case 13 : //输入回车开启输入窗口发消息
                if (exit_flag == 1) break;
                send_chat();
                break;
            case '.' :
                logout(); //退出游戏
                break;
            case '-' :
                music_dir = -1;
                move_music();
                break;
            case '+' :
                music_dir = 1;
                move_music();
                break;
            case '*' :
                if (bgm_flag == 0) {
                    show_message(&root_user, "抱歉，配置文件的bgm开关没有打开.不能播放音乐.", 1);
                    break;
                }
                if (stop_bgm_flag == 1) {
                    stop_bgm_flag = 0;
                    pthread_mutex_unlock(&stop_music_lock);
                }
                current_music_id = music_ind;
                sys_off();
                break;
            case '/' :
                if (stop_bgm_flag == 1) break;
                
                draw_now_music(0); //清除正在播放窗口的音乐
                
                pthread_mutex_lock(&stop_music_lock);
                stop_bgm_flag = 1;
                sys_off();
                break;
            default:
                break;
        }
    }
}
