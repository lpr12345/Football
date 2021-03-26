/*************************************************************************
	> File Name: send_map.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 17时03分36秒
 ************************************************************************/

#ifndef _SEND_MAP_H
#define _SEND_MAP_H
#include "../../Common/datatype.h"
#include "../../Common/head.h"

void send_to_user(FootBallMsg *msg, int flag);
void *send_map(void *arg);
void send_time();

#endif
