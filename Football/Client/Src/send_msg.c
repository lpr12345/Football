/*************************************************************************
	> File Name: send_msg.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 17时44分37秒
 ************************************************************************/

#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../Include/game_map.h"
#include "../Include/write_log.h"

extern WINDOW *Write_win;
extern FootBallMsg chat_msg, ctl_msg;
extern int sockfd;
extern int sleep_speed;
extern FootBallMsg root_user;

extern pthread_mutex_t draw_lock;

extern char *error_log_path;

void send_chat() {
    echo();
    nocbreak();
    bzero(chat_msg.msg, sizeof(chat_msg.msg));
    char temp_msg[512] = {0};
    pthread_mutex_lock(&draw_lock);
    w_gotoxy_putc(Write_win, 2, 2, '>');
    mvwscanw(Write_win, 2, 3, "%[^\n]s", temp_msg);
    pthread_mutex_unlock(&draw_lock);
    
    if (strlen(temp_msg) > 128) {
        show_message(&root_user, "消息太长无法发送.抱歉.", 1);
        pthread_mutex_lock(&draw_lock);
        wclear(Write_win);
        wattron(Write_win, COLOR_PAIR(5));
        box(Write_win, 0, 0);
        wattron(Write_win, COLOR_PAIR(3));
        w_gotoxy_puts(Write_win, 2, 1, "输入消息 : ");
        pthread_mutex_unlock(&draw_lock);
        cbreak();
        noecho();
        return ;
    }

    chat_msg.flag = 1;
    int len = strlen(temp_msg);
    if (temp_msg[0] == '@') { 
        //“　@:name　”　表示对某人私聊
        char temp_name[20] = {0};
        int j = 0;
        for (int i = 1; i < len; i++) {
            if (temp_msg[i] != ':') {
                temp_name[j++] = temp_msg[i];
            } else {
                strcpy(chat_msg.to_name, temp_name);
                for (int k = j + 2, l = 0; l < len; k++) {
                    if (k < len) chat_msg.msg[l++] = temp_msg[k];
                    else chat_msg.msg[l++] = '\0';
                }
                break;
            }
        }
        if (strlen(chat_msg.msg)) {
            chat_msg.flag = 3;
        }
    } else if (temp_msg[0] == '#' && temp_msg[1] == ':') { 
        //“　#:　”:表示对自己队伍的人发消息
        for (int k = 2, l = 0; l < len; k++) {
            if (k < len) chat_msg.msg[l++] = temp_msg[k];
            else chat_msg.msg[l++] = '\0';
        }
        if (strlen(chat_msg.msg)) {
            chat_msg.flag = 2;
        }
    } else strcpy(chat_msg.msg, temp_msg);
    
    if (strlen(chat_msg.msg)) {
        if (send(sockfd, (char *)&chat_msg, sizeof(chat_msg), 0) < 0) {
            write_log(error_log_path, "[send()] [error] [process : %d] [message : %s]", getpid(), strerror(errno)); 
        }
    }
    
    pthread_mutex_lock(&draw_lock);
    w_gotoxy_puts(Write_win, 2, 2, BLANK);
    pthread_mutex_unlock(&draw_lock);
    
    cbreak();
    noecho();
    return ;
}

void *send_ctl(void *arg) {
    while (1) {
        if (ctl_msg.ctl.dirx || ctl_msg.ctl.diry) {
        
            send(sockfd, (char *)&ctl_msg, sizeof(ctl_msg), 0);
            
            ctl_msg.ctl.dirx = ctl_msg.ctl.diry = 0;
        
        }
        usleep(sleep_speed);
    }
}

void send_strength(int strength) {
    FootBallMsg temp_msg;
    temp_msg.type = FT_CTL;
    temp_msg.flag = 2;
    temp_msg.strength = strength;
    send(sockfd, (char *)&temp_msg, sizeof(temp_msg), 0);
    return ;
}

void send_stop_ball() {
    FootBallMsg temp_msg;
    temp_msg.type = FT_CTL;
    temp_msg.flag = 3;
    send(sockfd, (char *)&temp_msg, sizeof(temp_msg), 0);
    return ;
}
