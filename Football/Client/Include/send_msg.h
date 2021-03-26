/*************************************************************************
	> File Name: send_msg.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 17时42分09秒
 ************************************************************************/

#ifndef _SEND_MSG_H
#define _SEND_MSG_H
#include "../../Common/datatype.h"
#include "../../Common/head.h"

void send_chat();
void *send_ctl(void *arg);
void send_strength(int strength);
void send_stop_ball();
#endif
