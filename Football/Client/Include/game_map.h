/*************************************************************************
	> File Name: game_map.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 11时57分55秒
 ************************************************************************/

#ifndef _GAME_MAP_H
#define _GAME_MAP_H

#include "../../Common/datatype.h"
#include "../../Common/head.h"

WINDOW *create_newwin(int width, int heigth, int startx, int starty, int col);
void destroy_win(WINDOW *win);
void gotoxy_putc(int x, int y, int c);
void gotoxy_puts(int x, int y, char *s);
void w_gotoxy_putc(WINDOW *win, int x, int y, int c);
void w_gotoxy_puts(WINDOW *win, int x, int y, char *s);
void w_gotoxy_football_s(int x, int y, char *s);
void w_gotoxy_football_s_com(int x, int y, char *s);
void draw_football_field();
void init_help_win();
void init_music_win();
void init_power();
void sys_off();
void init_football();
void show_message(FootBallMsg *user, char *msg, int type);

#endif
