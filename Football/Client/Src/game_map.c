/*************************************************************************
	> File Name: game_map.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 11时47分08秒
 ************************************************************************/

#include "../../Common/datatype.h"
#include "../../Common/head.h"
#include "../Include/game_map.h"
#include "../Include/write_log.h"
#include "../Include/draw_game.h"

extern Map court;
extern pthread_mutex_t draw_lock;
extern pthread_mutex_t music_lock;

extern char *error_log_path;

extern int bgm_flag;

WINDOW *Football_win, *Message_win, *Help_win, *Score_win, *Write_win, *Football_son_win, *Power_win, *Watch_win, *Time_win, *Music_win, *Now_bgm_win;
struct dirent **namelist;
int music_id;
int current_music_id = -1;
int music_cnt;
int music_ind = 0;
int message_num = 1;
int current_page = 0;
int max_page = 0;
int music_dir = 1;

WINDOW *create_newwin(int width, int heigth, int startx, int starty, int col) {
    WINDOW *win = newwin(heigth, width * 2, starty, startx);
    wattron(win, COLOR_PAIR(col));
    box(win, 0, 0);
    wattron(win, COLOR_PAIR(3));
    move(LINES - 1, 1);
    wrefresh(win);
    return win;
}

void destroy_win(WINDOW *win) {
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(win);
    delwin(win);
    return ;
}

void gotoxy_putc(int x, int y, int c) {
    move(y, x);
    addch(c);
    refresh();
}

void gotoxy_puts(int x, int y, char *s) {
    move(y, x);
    addstr(s);
    refresh();
}

void w_gotoxy_putc(WINDOW *win, int x, int y, int c) {
    mvwaddch(win, y, x, c);
}

void w_gotoxy_puts(WINDOW *win, int x, int y, char *s) {
    mvwprintw(win, y, x, s);
    wrefresh(win);
}

void w_gotoxy_football_s(int x, int y, char *s) {
    mvwprintw(Football_win, y, x * 2, s);
}

void w_gotoxy_football_s_com(int x, int y, char *s) {
    mvwprintw(Football_win, y, x, s);
}

void draw_football_field() {
    int mid_y = (court.start.y + court.heigth + 4) / 2 - 3;

    for (int i = mid_y - 2; i <= mid_y + 2; i++) {
        w_gotoxy_football_s_com(court.start.x, i, "║");
        w_gotoxy_football_s_com(court.start.x + 1 + court.width * 2 - 4, i, "║");
    }
    w_gotoxy_football_s_com(court.start.x, mid_y - 3, "╔══");
    w_gotoxy_football_s_com(court.start.x, mid_y + 3, "╚══");
    w_gotoxy_football_s_com(court.start.x + court.width * 2 - 5, mid_y - 3, "══╗");
    w_gotoxy_football_s_com(court.start.x + court.width * 2 - 5, mid_y + 3, "══╝");
    
    wattron(Football_win, COLOR_PAIR(1));
    
    for (int i = court.start.y + 2; i < ((court.start.y + court.heigth) / 2 - 1); i++) {
        w_gotoxy_football_s_com(court.width - 1, i, "│");
        w_gotoxy_football_s_com(court.width, i, "│");
    }
    
    w_gotoxy_putc(Football_win, court.width - 1, (court.start.y + court.heigth) / 2 - 1, '+');
    w_gotoxy_putc(Football_win, court.width, (court.start.y + court.heigth) / 2 - 1, '+');
    
    for (int i = (court.start.y + court.heigth) / 2; i <= court.start.y + court.heigth - 5; i++) {
        w_gotoxy_football_s_com(court.width - 1, i, "│");
        w_gotoxy_football_s_com(court.width, i, "│");
    }
    
    for (int i = mid_y - 3; i <= mid_y + 3; i++) {
        w_gotoxy_football_s_com(court.width - 9, i, "│");
        w_gotoxy_football_s_com(court.width + 8, i, "│");
    }

    for (int i = mid_y - 7; i <= mid_y + 7; i++) {
        w_gotoxy_football_s_com(court.width * 2 - 20, i, "│");
        w_gotoxy_football_s_com(court.start.x + 18, i, "│");
    }
    
    for (int i = mid_y - 3; i <= mid_y + 3; i++) {
        w_gotoxy_football_s_com(court.width * 2 - 11, i, "│");
        w_gotoxy_football_s_com(court.start.x + 9, i, "│");
    }
    
    w_gotoxy_football_s_com(court.start.x + 4, mid_y - 4, "─────┐");
    w_gotoxy_football_s_com(court.start.x + 4, mid_y + 4, "─────┘");
    
    w_gotoxy_football_s_com(court.width * 2 - 11, mid_y - 4, "┌─────");
    w_gotoxy_football_s_com(court.width * 2 - 11, mid_y + 4, "└─────");

    w_gotoxy_football_s_com(court.start.x + 4, mid_y - 8, "──────────────┐");
    w_gotoxy_football_s_com(court.start.x + 4, mid_y + 8, "──────────────┘");
    
    w_gotoxy_football_s_com(court.width * 2 - 20, mid_y - 8, "┌───────────────");
    w_gotoxy_football_s_com(court.width * 2 - 20, mid_y + 8, "└───────────────");
    
    w_gotoxy_football_s_com(court.width - 9, mid_y - 4, "┌───────┼┼───────┐");
    w_gotoxy_football_s_com(court.width - 9, mid_y + 4, "└───────┼┼───────┘");
    wattron(Football_win, COLOR_PAIR(3));
}

void init_help_win() {
    w_gotoxy_puts(Help_win, 8, 1, "操作");
    
    wattron(Help_win, COLOR_PAIR(4));
    w_gotoxy_puts(Help_win, 8, 3, "移动");
    w_gotoxy_puts(Help_win, 3, 6, "停球|带球|踢球");
    w_gotoxy_puts(Help_win, 6, 9, "发送消息");
    w_gotoxy_puts(Help_win, 3, 14, "切歌|播放|停止");
    w_gotoxy_puts(Help_win, 8, 17, "退出");

    wattron(Help_win, COLOR_PAIR(1));
    w_gotoxy_puts(Help_win, 3, 4, "W A S D Q E Z C");
    w_gotoxy_puts(Help_win, 5, 7, "J|K|Space");
    w_gotoxy_puts(Help_win, 8, 10, "Enter");
    w_gotoxy_puts(Help_win, 4, 11, "私聊->\"@name:\"");
    w_gotoxy_puts(Help_win, 4, 12, "队伍聊天->\"#:\"");
    w_gotoxy_puts(Help_win, 6, 15, "+|- * /");
    w_gotoxy_puts(Help_win, 9, 18, ".");
    
    return ;
}

int filter_fn(const struct dirent * ent) {
    return (strncmp((ent->d_name + strlen(ent->d_name) - 3), "mp3", 3) == 0);
}

void init_music_win() {
    w_gotoxy_puts(Music_win, 5, 1, "音乐 ");
    if (bgm_flag) {
        wattron(Music_win, COLOR_PAIR(1));
        w_gotoxy_puts(Music_win, 10, 1, "ON");
    } else {
        wattron(Music_win, COLOR_PAIR(2));
        w_gotoxy_puts(Music_win, 10, 1, "OFF");
    }
    wattron(Music_win, COLOR_PAIR(4));
    music_cnt = scandir("./Bgm/", &namelist, filter_fn, alphasort);
    max_page = (music_cnt / 43) + ((music_cnt == 43 * (music_cnt / 43)) ? 0 : 1);
    for (int i = 0; i < music_cnt && i < 44; i++) {
        int len = strlen(namelist[i]->d_name);
        char temp_buff[25] = {0};
        for (int j = 4, k = 0; j < len - 4; j++) {
            temp_buff[k++] = namelist[i]->d_name[j];
        }
        w_gotoxy_puts(Music_win, 2, i + 3, temp_buff);
    }
    wattron(Music_win, COLOR_PAIR(3));
    char temp_buff[20] = {0};
    sprintf(temp_buff, "page : %d", current_page + 1);
    w_gotoxy_puts(Music_win, 4, 47, temp_buff);
 
    draw_music_arrow(0, 0);

    wattron(Music_win, COLOR_PAIR(5));
    box(Music_win, 0, 0);
    wattron(Music_win, COLOR_PAIR(3));

    wrefresh(Music_win);
    return ;
}

void sys_off() {
    char temp_buff[128] = {0};
    pthread_mutex_lock(&music_lock);
    sprintf(temp_buff, "kill -9 `ps -v | grep ./Bgm/%03d | awk '{print $1}'` 2>/dev/null", music_id);
    system(temp_buff);
    pthread_mutex_unlock(&music_lock);
}

void init_power() {
    for (int i = 1; i <= 5; i++) {
        if (i == 1 || i == 5) wattron(Power_win, COLOR_PAIR(7));
        else if (i == 2 || i == 4 ) wattron(Power_win, COLOR_PAIR(10));
        else wattron(Power_win, COLOR_PAIR(8));
        w_gotoxy_puts(Power_win, (i - 1) * 13 + 3, 2, POWER_BLANK);
        wattron(Power_win, COLOR_PAIR(3));
    }
    wrefresh(Power_win);
    return ;
}

void init_football() {
    initscr();
    clear();

    curs_set(0); //参数为0，隐藏光标

    if (!has_colors() || start_color() == ERR) {
        write_log(error_log_path, "[has_colors()] [error] [process : %d] [message : %s]", getpid(), strerror(errno));
        endwin();
        exit(1);
    }

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    
    init_pair(7, COLOR_BLACK, COLOR_GREEN);
    init_pair(8, COLOR_BLACK, COLOR_RED);
    init_pair(9, COLOR_BLACK, COLOR_BLUE);
    init_pair(10, COLOR_BLACK, COLOR_YELLOW);
    init_pair(11, COLOR_BLACK, COLOR_CYAN);
    
    init_pair(12, COLOR_BLUE, COLOR_WHITE);

    Football_win = create_newwin(court.width, court.heigth, court.start.x, court.start.y, 5);
    draw_football_field();
    Football_son_win = subwin(Football_win, court.heigth - 4, court.width * 2 - 8, court.start.y + 2, court.start.x + 4);

    wattron(Football_son_win, COLOR_PAIR(1));
    box(Football_son_win, 0, 0);
    wrefresh(Football_son_win);
    
    Message_win = create_newwin(court.width, 8, court.start.x, court.start.y + court.heigth , 5);
    w_gotoxy_puts(Message_win, 2, 1, "聊天窗口 : ");
    Message_win = subwin(Message_win, 5, court.width * 2 - 2, court.start.y + court.heigth + 2, court.start.x + 1);
    scrollok(Message_win, 1);
    
    Help_win = create_newwin(10, court.heigth - 10, court.start.x + court.width * 2, court.start.y + 5, 5);
    init_help_win();
    
    Time_win = create_newwin(10, 5, court.start.x + court.width * 2, court.start.y + court.heigth - 5, 5);
    wattron(Time_win, COLOR_PAIR(3));
    w_gotoxy_puts(Time_win, 6, 1, "剩余时间");
    wattron(Time_win, COLOR_PAIR(4));

    Score_win = create_newwin(10, 8, court.start.x + court.width * 2, court.start.y + court.heigth, 5);
    wattron(Score_win, COLOR_PAIR(2));
    w_gotoxy_puts(Score_win, 3, 2, "红队得分 : 0");
    wattron(Score_win, COLOR_PAIR(6));
    w_gotoxy_puts(Score_win, 3, 5, "蓝队得分 : 0");
    
    Write_win = create_newwin(court.width + 10, 5, court.start.x, court.start.y + court.heigth + 8, 5);
    w_gotoxy_puts(Write_win, 2, 1, "输入消息 : ");
    
    Power_win = create_newwin(court.width / 2 + 5, 5, court.start.x, court.start.y + court.heigth + 13, 5);
    init_power();
    
    Watch_win = create_newwin(court.width / 2 + 6, 5, court.start.x + 72, court.start.y + court.heigth + 13, 5);
    re_draw_watch();

    Music_win = create_newwin(9, court.heigth + 18, court.start.x + court.width * 2 + 20, court.start.y, 5);
    init_music_win();
    
    Now_bgm_win = create_newwin(10, 5, court.start.x + court.width * 2, court.start.y, 5);
    w_gotoxy_puts(Now_bgm_win, 6, 1, "当前歌曲");
}

void show_message(FootBallMsg *user, char *msg, int type) {
    time_t time_now = time(NULL);
    struct tm* t = localtime(&time_now);
    char timestr[20] = {0};
    char user_name[40] = {0};
    sprintf(timestr, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    if (type) {
        wattron(Message_win, COLOR_PAIR(4));
        sprintf(user_name, "server %s : ", user->name);
    } else {
        if (user->team == 1) wattron(Message_win, COLOR_PAIR(6));
        else if (user->team == 0) wattron(Message_win, COLOR_PAIR(2));
        else if (user->team == 2) wattron(Message_win, COLOR_PAIR(1));
        
        if (user->flag == 1) {
            sprintf(user_name, "%s : ", user->name);    
        } else if (user->flag == 2) {
            sprintf(user_name, "%s 对队伍说 : ", user->name);
        } else if (user->flag == 3) {
            sprintf(user_name, "%s 对你说 : ", user->name); 
        }
    }

    if (message_num >= 5) {
        message_num = 4;
        scroll(Message_win);
    }

    pthread_mutex_lock(&draw_lock);
    w_gotoxy_puts(Message_win, 10, message_num, user_name);
    wattron(Message_win, COLOR_PAIR(3));
    w_gotoxy_puts(Message_win, 10 + strlen(user_name), message_num, msg);
    wattron(Message_win, COLOR_PAIR(5));
    w_gotoxy_puts(Message_win, 1, message_num, timestr);
    message_num++;
    pthread_mutex_unlock(&draw_lock);
    return ;
}
