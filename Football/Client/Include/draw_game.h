/*************************************************************************
	> File Name: draw_game.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 15时54分36秒
 ************************************************************************/

#ifndef _DRAW_GAME_H
#define _DRAW_GAME_H

#include "../../Common/datatype.h"
#include "../../Common/head.h"

void re_draw_player(int team, char *id, struct Point *loc);
void re_draw_team(struct User_t *team);
void *re_draw(void *arg);
void re_draw_music(int id);
void draw_music_arrow(int last_ind, int id);
void draw_now_music(int flag);
void move_music();
void show_strength();
void draw_exit_win();
void draw_exit_user(Exit_user_msg *exit_user);
void re_draw_watch();

#endif
