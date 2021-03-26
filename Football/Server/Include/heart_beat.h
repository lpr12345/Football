/*************************************************************************
	> File Name: heart_beat.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月04日 星期三 19时57分41秒
 ************************************************************************/

#ifndef _HEART_BEAT_H
#define _HEART_BEAT_H

#include "../../Common/datatype.h"

void heart_beat_team(User *user, int epollfd_tmp);
void *heart_beat(void *arg);

#endif
