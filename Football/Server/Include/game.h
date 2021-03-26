/*************************************************************************
	> File Name: game.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 11时38分53秒
 ************************************************************************/

#ifndef _GAME_H
#define _GAME_H

#include "../../Common/head.h"
#include "../../Common/datatype.h"

void ball_zero();
void *re_ball(void *arg);
int can_kick(Point *loc, int user_team, int strength);
int can_stop(Point *loc);


#endif
