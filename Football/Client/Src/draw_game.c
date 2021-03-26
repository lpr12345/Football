/*************************************************************************
	> File Name: draw_game.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 15时55分49秒
 ************************************************************************/

#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../Include/game_map.h"
#include "../Include/send_msg.h"
#include "../Include/write_log.h"

extern Map court;
extern WINDOW *Football_win, *Football_son_win, *Power_win, *Music_win, *Write_win, *Now_bgm_win, *Watch_win;

extern User_t *rteam, *bteam;
extern pthread_mutex_t draw_lock;
extern Point ball;

extern Watch_user watch_user[MAX];

extern char *error_log_path;

extern struct dirent **namelist;
extern int current_page;
extern int max_page;
extern int music_cnt;
extern int current_music_id;
extern int music_dir;
extern int bgm_flag;
extern int exit_flag;
extern int last_id;
extern int music_ind;
extern int music_id;
int last_ind = 1;
int last_music_id = 0;
int watch_map[10][2] = {{1, 2}, {15, 2}, {29, 2}, {43, 2}, {57, 2}, {1, 3}, {15, 3}, {29, 3}, {43, 3}, {57, 3}};

void re_draw_player(int team, char *id, Point *loc) {
    pthread_mutex_lock(&draw_lock);
    
    if (team) wattron(Football_win, COLOR_PAIR(9));
    else wattron(Football_win, COLOR_PAIR(8));
    w_gotoxy_football_s(loc->x, loc->y, id);
    
    pthread_mutex_unlock(&draw_lock);
    return ;
}

void re_draw_team(User_t *team) {
    for (int i = 0; i < MAX; i++) {
        if (!team[i].online) continue;
        re_draw_player(team[i].team, team[i].id, &team[i].loc);       
    }
    return ;
}

void *re_draw(void *arg) {
    while (1) {
        pthread_mutex_lock(&draw_lock);
        wclear(Football_win);
        wattron(Football_win, COLOR_PAIR(5));
        box(Football_win, 0, 0);
        wattron(Football_win, COLOR_PAIR(3));
        draw_football_field();
        wattron(Football_son_win, COLOR_PAIR(1));
        box(Football_son_win, 0, 0);
        wrefresh(Football_son_win);

        w_gotoxy_football_s(ball.x, ball.y, "🌐 ");
        wrefresh(Football_win);
        pthread_mutex_unlock(&draw_lock);

        re_draw_team(rteam);
        re_draw_team(bteam);
        
        pthread_mutex_lock(&draw_lock);
        wrefresh(Football_win);
        wattron(Football_son_win, COLOR_PAIR(3));
        pthread_mutex_unlock(&draw_lock);
        
        usleep(100000);  
    }
}

void re_draw_music() {
    pthread_mutex_lock(&draw_lock);
	wclear(Music_win);
    wattron(Music_win, COLOR_PAIR(5));
    box(Music_win, 0, 0);
    wattron(Music_win, COLOR_PAIR(3));
	w_gotoxy_puts(Music_win, 5, 1, "音乐 ");
    if (bgm_flag) {
        wattron(Music_win, COLOR_PAIR(1));
        w_gotoxy_puts(Music_win, 10, 1, "ON");
    } else {
        wattron(Music_win, COLOR_PAIR(2));
        w_gotoxy_puts(Music_win, 10, 1, "OFF");
    }

    wattron(Music_win, COLOR_PAIR(4));
    for (int i = 0; i + current_page * 44 < music_cnt && i < 44; i++) {
        int len = strlen(namelist[i + current_page * 44]->d_name);
        char temp_buff[25] = {0};
        for (int j = 4, k = 0; j < len - 4; j++) {
            temp_buff[k++] = namelist[i + current_page * 44]->d_name[j];
        }
        if (i + current_page * 44 == music_id) {
            wattron(Music_win, COLOR_PAIR(6));
            w_gotoxy_puts(Music_win, 2, i + 3, temp_buff);
            wattron(Music_win, COLOR_PAIR(4));
            continue;
        }
        w_gotoxy_puts(Music_win, 2, i + 3, temp_buff);
    }
    wattron(Music_win, COLOR_PAIR(3));
    char temp_buff[20] = {0};
    sprintf(temp_buff, "page : %d", current_page + 1);
    w_gotoxy_puts(Music_win, 4, 47, temp_buff);
    pthread_mutex_unlock(&draw_lock);
 
    return ;
}

void draw_music_arrow(int last_id, int id) {
    static int temp_page = 0;
    
    pthread_mutex_lock(&draw_lock);
    
    if (temp_page != current_page) {
        pthread_mutex_unlock(&draw_lock);
        re_draw_music();
        temp_page = current_page;
        pthread_mutex_lock(&draw_lock);
    }
    
    w_gotoxy_puts(Music_win, 1, last_id + 3, " ");
    w_gotoxy_puts(Music_win, 16, last_id + 3, " ");
    
    wattron(Music_win, COLOR_PAIR(12));
    w_gotoxy_puts(Music_win, 1, id + 3 - current_page * 44, ">");
    w_gotoxy_puts(Music_win, 16, id + 3 - current_page * 44, "<");
    wattron(Music_win, COLOR_PAIR(3));
    wrefresh(Music_win);
    pthread_mutex_unlock(&draw_lock);
    return ;
}

void draw_now_music(int flag) {
    pthread_mutex_lock(&draw_lock);
    wclear(Now_bgm_win);
    
    wattron(Now_bgm_win, COLOR_PAIR(5));
    box(Now_bgm_win, 0, 0);
    wattron(Now_bgm_win, COLOR_PAIR(3));
    
    w_gotoxy_puts(Now_bgm_win, 6, 1, "当前音乐");

    if (flag == 0) {
        pthread_mutex_unlock(&draw_lock);
        return ;
    }

    wattron(Now_bgm_win, COLOR_PAIR(4));

    int len = strlen(namelist[music_id]->d_name);
    char temp_buff[25] = {0};
    for (int j = 4, k = 0; j < len - 4; j++) {
        temp_buff[k++] = namelist[music_id]->d_name[j];
    }
    
    len = (len - 4) / 3;
    w_gotoxy_puts(Now_bgm_win, 11 - len , 3, temp_buff);   

    wattron(Now_bgm_win, COLOR_PAIR(3));

    pthread_mutex_unlock(&draw_lock);

    re_draw_music();

    draw_music_arrow(last_ind, music_ind);
}


void move_music() {
    if (music_ind % 44 == 0 && music_dir == -1) {
        current_page--;
        if (current_page == -1) current_page = max_page - 1;
    } else if ((music_ind == music_cnt - 1 && music_dir == 1) || (music_ind != 0 && music_ind % 44 == 43 && music_dir == 1)) {
        current_page++;
        if (current_page == max_page) current_page = 0;
    }

    int last_id = music_ind % 44;
    music_ind = music_ind + music_dir;
        
    if (music_ind == music_cnt) {
        music_ind = 0;
    } else if (music_ind == -1) {
        music_ind = music_cnt - 1;
    }
    
    draw_music_arrow(last_id, music_ind);
    return ;
}


void show_strength() {
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    
    int temp_dir = 1, strength = 0;
    for (int i = 1; ; i += temp_dir) {
        pthread_mutex_lock(&draw_lock);
        
        w_gotoxy_putc(Power_win, i + 2 - temp_dir, 3, ' ');
        w_gotoxy_putc(Power_win, i + 2, 3, '^');
        wrefresh(Power_win);
        
        pthread_mutex_unlock(&draw_lock);
        
        if (i == 65) temp_dir = -1;
        else if (i == 1) temp_dir = 1;
        char temp_c = '\0';
        temp_c = getchar();
        int arr[5] = {10, 15, 20, 15, 10};
        if (temp_c == ' ') {
            last_ind = i + 2;
            strength = arr[i / (65 / 5)];
            break;
        }
        usleep(25000);
    }
    
    pthread_mutex_lock(&draw_lock);
    w_gotoxy_putc(Power_win, last_ind, 3, ' ');
    pthread_mutex_unlock(&draw_lock);
        
    wrefresh(Power_win);

    int flags = fcntl(0, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(0, F_SETFL, flags);
    
    send_strength(strength);
    return ;
}

void draw_exit_win() {
    pthread_mutex_lock(&draw_lock);
    
    wclear(Football_win);
    wattron(Football_win, COLOR_PAIR(5));
    box(Football_win, 0, 0);
    wattron(Football_win, COLOR_PAIR(4));
    w_gotoxy_football_s_com(court.width - 6, court.start.y + 1, "游戏已结束.");
    wrefresh(Football_win);
    
    pthread_mutex_unlock(&draw_lock);
    return ;
}

void draw_exit_user(Exit_user_msg *exit_user) {
    pthread_mutex_lock(&draw_lock);
    wattron(Football_win, COLOR_PAIR(3));
    
    for (int i = 3; i < 28; i += 2) {
        if (i == 3) w_gotoxy_football_s_com(court.start.x + 15 , court.start.y + i, "┌───────────────┬────────┬────────┐");
        else if (i == 27) w_gotoxy_football_s_com(court.start.x + 15 , court.start.y + i, "└───────────────┴────────┴────────┘");
        else w_gotoxy_football_s_com(court.start.x + 15 , court.start.y + i, "├───────────────┼────────┼────────┤");
        
        if (i != 27) w_gotoxy_football_s_com(court.start.x + 15 , court.start.y + i + 1, "│               │        │        │");
    }
    
    for (int i = 3; i < 28; i += 2) {
        if (i == 3) w_gotoxy_football_s_com(court.start.x + 75 , court.start.y + i, "┌───────────────┬────────┬────────┐");
        else if (i == 27) w_gotoxy_football_s_com(court.start.x + 75 , court.start.y + i, "└───────────────┴────────┴────────┘");
        else w_gotoxy_football_s_com(court.start.x + 75 , court.start.y + i, "├───────────────┼────────┼────────┤");
        
        if (i != 27) w_gotoxy_football_s_com(court.start.x + 75 , court.start.y + i + 1, "│               │        │        │");
    }

    wattron(Football_win, COLOR_PAIR(1));

    w_gotoxy_football_s_com(court.start.x + 22 , court.start.y + 4, "姓名");
    w_gotoxy_football_s_com(court.start.x + 34 , court.start.y + 4, "进球");
    w_gotoxy_football_s_com(court.start.x + 43 , court.start.y + 4, "乌龙");
    
    w_gotoxy_football_s_com(court.start.x + 82 , court.start.y + 4, "姓名");
    w_gotoxy_football_s_com(court.start.x + 94 , court.start.y + 4, "进球");
    w_gotoxy_football_s_com(court.start.x + 103 , court.start.y + 4, "乌龙");
    
    wrefresh(Football_win);

    for (int i = 0, j = 6, k = 6; i < MAX; i++) {
        if (exit_user->rteam[i].online) {
            wattron(Football_win, COLOR_PAIR(2));
            char buff[5] = {0};
            w_gotoxy_football_s_com(court.start.x + 16 , court.start.y + j, exit_user->rteam[i].name);
            
            sprintf(buff, "%d", exit_user->rteam[i].score);
            w_gotoxy_football_s_com(court.start.x + 32 , court.start.y + j, buff);
            
            sprintf(buff, "%d", exit_user->rteam[i].own);
            w_gotoxy_football_s_com(court.start.x + 41 , court.start.y + j, buff);

            wrefresh(Football_win);

            j += 2;
        } 
        
        if (exit_user->bteam[i].online) {
            wattron(Football_win, COLOR_PAIR(6));
            char buff[5] = {0};
            w_gotoxy_football_s_com(court.start.x + 76, court.start.y + k, exit_user->bteam[i].name);
            
            sprintf(buff, "%d", exit_user->bteam[i].score);
            w_gotoxy_football_s_com(court.start.x + 92 , court.start.y + k, buff);
            
            sprintf(buff, "%d", exit_user->bteam[i].own);
            w_gotoxy_football_s_com(court.start.x + 101 , court.start.y + k, buff);
            
            wrefresh(Football_win);

            k += 2;
        } 
    }
    pthread_mutex_unlock(&draw_lock);
    return ;
}

void re_draw_watch() {
    pthread_mutex_lock(&draw_lock);
    wclear(Watch_win);
    wattron(Watch_win, COLOR_PAIR(5));
    box(Watch_win, 0, 0);
    
    wattron(Watch_win, COLOR_PAIR(3));
    w_gotoxy_puts(Watch_win, 34, 1, "观众席");

    wattron(Watch_win, COLOR_PAIR(1));
    for (int i = 0, j = 0; i < MAX; i++) {
        if (watch_user[i].online) {
            w_gotoxy_puts(Watch_win, watch_map[j][0], watch_map[j][1], watch_user[i].name);
            j++;
        }
    }

    pthread_mutex_unlock(&draw_lock);
    return ;
}
