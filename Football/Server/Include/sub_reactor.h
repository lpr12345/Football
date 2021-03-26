/*************************************************************************
	> File Name: sub_reactor.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 12时33分40秒
 ************************************************************************/

#ifndef _SUB_REACTOR_H
#define _SUB_REACTOR_H

#include "../../Common/datatype.h"
#include "../../Common/head.h"

int find_ind(User *team);

void add_to_sub_reactor(User *User);

void *sub_reactor(void *arg);

#endif
